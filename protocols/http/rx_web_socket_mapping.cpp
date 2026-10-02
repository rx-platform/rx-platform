

/****************************************************************************
*
*  protocols\http\rx_web_socket_mapping.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_web_socket_mapping
#include "protocols/http/rx_web_socket_mapping.h"

#include "protocols/http/rx_http_mapping.h"



namespace protocols {

namespace rx_http {

// Class protocols::rx_http::rx_web_socket_endpoint 

rx_web_socket_endpoint::rx_web_socket_endpoint (rx_web_socket_port* port)
      : port_(port),
        executer_(-1),
        state_(web_socket_state::initial),
        head_size_(0),
        payload_to_read_(0)
    , parser_(true)
{
    HTTP_LOG_DEBUG("rx_web_socket_endpoint", 200, "HTTP WS communication server endpoint created.");
    rx_init_stack_entry(&stack_entry, this);
    stack_entry.received_function = &rx_web_socket_endpoint::received_function;
    stack_entry.send_function = &rx_web_socket_endpoint::send_function;

	data_collector_ = std::make_unique<rx_web_socket_data_packet_collector>(&stack_entry);

    executer_ = port->get_executer();
    parser_.callback = [this](http_parsed_request req) -> rx_protocol_result_t
        {
			return parse_http_request(std::move(req));
        };
}


rx_web_socket_endpoint::~rx_web_socket_endpoint()
{
    HTTP_LOG_DEBUG("rx_web_socket_endpoint", 200, "HTTP WS communication server endpoint destroyed.");
}



rx_protocol_result_t rx_web_socket_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    rx_web_socket_endpoint* self = reinterpret_cast<rx_web_socket_endpoint*>(reference->user_data);
    self->port_->status.received_packet();
    if (self->state_ == web_socket_state::initial)
    {
        auto result = self->parser_.received(packet);
        if(result != RX_PROTOCOL_OK)
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to parse HTTP request for WebSocket Upgrade: "s
                + rx_protocol_error_message(result));
		}
		return result;
    }
    else
    {
        return self->received(packet);
    }
}

rx_protocol_result_t rx_web_socket_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    rx_web_socket_endpoint* self = reinterpret_cast<rx_web_socket_endpoint*>(reference->user_data);
    if (self->state_ == web_socket_state::initial)
    {
		return RX_PROTOCOL_INVALID_SEQUENCE; // Cannot send data before the handshake is complete
    }
    else
    {
        return self->send(packet);
    }
}

void rx_web_socket_endpoint::close_endpoint ()
{
}

rx_protocol_result_t rx_web_socket_endpoint::parse_http_request (http_parsed_request req)
{
    if(req.method != rx_http_method::get)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Invalid HTTP method for WebSocket Upgrade request.");
        return RX_PROTOCOL_INVALID_ARG;
	}
    string_view_type conn = req.headers["Connection"];
    string_view_type upgrade = req.headers["Upgrade"];
    string_view_type web_key = req.headers["Sec-WebSocket-Key"];
    if (!conn.empty() && !upgrade.empty() && !web_key.empty())
    {
        if(conn.find("Upgrade") != string_view_type::npos && upgrade.compare("websocket") == 0)
        {
            // WebSocket Upgrade request
            HTTP_LOG_DEBUG("rx_web_socket_endpoint", 200, "WebSocket Upgrade request received.");
            // Prepare response
            string_type response_key = generate_websocket_accept(string_type(web_key));
            string_type response;
			response += "HTTP/1.1 101 Switching Protocols\r\n";
			response += "Server: " + rx_internal::rx_http_server::http_server::get_server_header_info() + "\r\n";
			response += "Upgrade: websocket\r\n";
			response += "Connection: Upgrade\r\n";
			response += "Sec-WebSocket-Accept: " + response_key + "\r\n";
			response += "\r\n"; // End of headers

            auto packet_buff = port_->alloc_io_buffer();
			packet_buff.value().write_chars(response);
            // Send response
            auto send_packet = rx_create_send_packet(0, &packet_buff.value(), 0, 0);

            auto result = rx_move_packet_down(&stack_entry, send_packet);
            if (result != RX_PROTOCOL_OK)
            {
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to send WebSocket Upgrade response.");
                return result;
            }
            io::string_address remote_address("");
            io::string_address addr(string_type(req.path));
            auto conn_result = port_->stack_endpoint_connected(&stack_entry, &addr, &remote_address);
            if (!conn_result)
            {
                std::ostringstream ss;
                ss << "Error binding connected endpoint ";
                ss << conn_result.errors_line();
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 200, ss.str().c_str());
                return RX_PROTOCOL_INVALID_ADDR;
            }
			state_ = web_socket_state::idle;
            return RX_PROTOCOL_OK;
        }
        else
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Invalid WebSocket Upgrade request.");
		}
        return RX_PROTOCOL_OK;
    }
    return RX_PROTOCOL_INVALID_ARG;
}

string_type rx_web_socket_endpoint::generate_websocket_accept (const string_type& web_key)
{
    // Generate the WebSocket accept key as per RFC 6455
    // The key is base64 encoded SHA-1 of the concatenation of the WebSocket key and the magic string "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    string_type magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    string_type combined_key = web_key + magic_string;
    // Compute SHA-1 hash
    auto hash = rx_crypt_create_hash(0, RX_HASH_SHA1);
    if(hash == 0)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to create hash for WebSocket accept key generation.");
        return string_type();
	}
	auto hash_result = rx_crypt_hash_data(hash, combined_key.c_str(), combined_key.size());
    if (hash_result != RX_OK)
    {
        rx_crypt_destroy_hash(hash);
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to hash data for WebSocket accept key generation.");
        return string_type();
    }
	// Get the hash result
    uint8_t buffer[0x20];
	size_t size = sizeof(buffer);
    hash_result = rx_crypt_get_hash(hash, &buffer, &size);
    if(hash_result != RX_OK || size != 20) // SHA-1 produces 20 bytes
    {
        rx_crypt_destroy_hash(hash);
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to retrieve hash for WebSocket accept key generation.");
        return string_type();
	}
    rx_crypt_destroy_hash(hash);
    // Base64 encode the hash
	string_value_struct sha1_base64;
    if(RX_OK != rx_base64_get_string(&sha1_base64, buffer, size))
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to generate WebSocket accept key.");
        return string_type();
    }
    string_type ret = rx_c_str(&sha1_base64);
    rx_destory_string_value_struct(&sha1_base64);
    return ret;
}

rx_protocol_result_t rx_web_socket_endpoint::received (recv_protocol_packet packet)
{

    if (packet.buffer != nullptr)
    {
        io::rx_const_io_buffer buffer(packet.buffer);
        while (!buffer.eof())
        {
			size_t available = buffer.available_size();
            switch(state_)
            {
                case web_socket_state::idle:
                    RX_ASSERT(head_size_ == 0);
					[[fallthrough]]; // Intentional fall-through to header_collect
                case web_socket_state::header_collect:
                    {
                        RX_ASSERT(head_size_ < sizeof(header_buffer_) / sizeof(header_buffer_[0]));

                        RX_ASSERT(payload_to_read_ == 0);
                        bool head_done = false;
                        rx_result result;
                        do
                        {
                            if (head_size_ >= sizeof(header_buffer_))
                                return RX_PROTOCOL_INVALID_SEQUENCE;

                            result = buffer.read_from_buffer(header_buffer_[head_size_]);
                            if (!result)
                            {
                                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to read first byte from WebSocket frame:" + result.errors_line());
                                return RX_PROTOCOL_PARSING_ERROR;
                            }
                            head_size_ += 1;
							head_done = is_header_done();
                        } while (head_size_ < available && !head_done);
                        // Read the first byte (opcode) and the second byte (payload length)
                        if (!head_done)
                        {
                            state_ = web_socket_state::header_collect;
                        }
                        else
                        {
                            available = buffer.available_size();
                            auto new_head = parse_header();
                            head_size_ = 0;
                            if(!new_head)
                            {
                                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Invalid WebSocket frame header received.");
                                return RX_PROTOCOL_PARSING_ERROR;
							}
                            current_header_ = new_head;
							auto frame_res = handle_ws_frame(buffer, available);
                            if (frame_res != RX_PROTOCOL_OK)
                            {
                                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle WebSocket frame: "s + rx_protocol_error_message(frame_res));
								return frame_res;
                            }
                        }
			        }
                    break;
                case web_socket_state::payload_collect:
                    {
                        auto frame_res = handle_ws_frame(buffer, available);
                        if (frame_res != RX_PROTOCOL_OK)
                        {
                            HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle WebSocket frame: "s + rx_protocol_error_message(frame_res));
                            return frame_res;
                        }
                    }
                    break;
                default:
                    {
                    }
                    break;
            }
        }
    }

    return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_web_socket_endpoint::send (send_protocol_packet packet)
{
    if (packet.buffer == nullptr || packet.buffer->size == 0)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Cannot send empty WebSocket frame.");
        return RX_PROTOCOL_INVALID_ARG;
    }
    // Prepare the WebSocket frame header
    uint8_t header[10] = {0}; // Maximum header size for WebSocket frame
    size_t header_size = 2; // Initial size for the first two bytes
    // Set the final fragment bit and opcode (text frame)
    header[0] = 0x81; // 10000001 - Final fragment, Text frame
    // Set payload length
    size_t payload_length = packet.buffer->size;
    if (payload_length <= 125)
    {
        header[1] = static_cast<uint8_t>(payload_length);
    }
    else if (payload_length <= 65535)
    {
        header[1] = 126; // Extended payload length
        header[2] = static_cast<uint8_t>((payload_length >> 8) & 0xFF);
        header[3] = static_cast<uint8_t>(payload_length & 0xFF);
        header_size += 2; // Add two bytes for extended length
    }
    else
    {
        header[1] = 127; // Extended payload length
        header[2] = static_cast<uint8_t>((payload_length >> 56) & 0xFF);
        header[3] = static_cast<uint8_t>((payload_length >> 48) & 0xFF);
        header[4] = static_cast<uint8_t>((payload_length >> 40) & 0xFF);
        header[5] = static_cast<uint8_t>((payload_length >> 32) & 0xFF);
        header[6] = static_cast<uint8_t>((payload_length >> 24) & 0xFF);
        header[7] = static_cast<uint8_t>((payload_length >> 16) & 0xFF);
        header[8] = static_cast<uint8_t>((payload_length >> 8) & 0xFF);
        header[9] = static_cast<uint8_t>(payload_length & 0xFF);
        header_size += 8; // Add eight bytes for extended length
    }
    rx_protocol_result_t result = rx_push_to_packet_front(packet.buffer, header, header_size); // Push the header to the front of the packet buffer
    if (result != RX_PROTOCOL_OK)
		return result; // If pushing to packet front fails, return the error
    return rx_move_packet_down(&stack_entry, packet);
}

bool rx_web_socket_endpoint::is_header_done ()
{
    if(head_size_ < 2)
    {
        return false; // Header is not complete
	}
	size_t mask_size = (header_buffer_[1] & 0x80) ? 4 : 0; // Check if mask is present
    switch(header_buffer_[1]& 0x7F) // Mask the first bit to get payload length
    {
        case 126: // Extended payload length (16 bits)
            if(head_size_ < 4 + mask_size) // We need at least 4 bytes for this case
            {
                return false;
            }
            break;
        case 127: // Extended payload length (64 bits)
            if(head_size_ < 10 + mask_size) // We need at least 10 bytes for this case
            {
                return false;
            }
            break;
        default: // Normal payload length (0-125)
            if (head_size_ < 2 + mask_size) // We need at least 10 bytes for this case
            {
                return false;
            }
            break;
	}
	return true; // Header is complete
}

web_socket_header rx_web_socket_endpoint::parse_header ()
{
	web_socket_header header;
	memzero(&header, sizeof(header)); // Initialize header to zero
	header.is_final = (header_buffer_[0] & 0x80) != 0; // Check if the final fragment bit is set
	header.opcode = static_cast<ws_opcode>(header_buffer_[0] & 0x0F); // Mask to get the opcode only
	header.is_masked = (header_buffer_[1] & 0x80) != 0; // Check if the mask bit is set
	size_t next_byte_index = 2; // Start reading from the third byte
    switch (header_buffer_[1] & 0x7F)
    {
        case 126: // Extended payload length (16 bits)
            if (head_size_ < 4) // We need at least 4 bytes for this case
            {
                header.opcode = ws_opcode::invalid_opcode; // Set invalid opcode
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "WebSocket header is incomplete for extended payload length.");
                return header; // Return an empty header
            }
            header.payload_length = (header_buffer_[2] << 8) | header_buffer_[3]; // Combine the two bytes
			next_byte_index = 4; // Move to the next byte after payload length
			break;
        case 127: // Extended payload length (64 bits)
            if (head_size_ < 10) // We need at least 10 bytes for this case
            {
                header.opcode = ws_opcode::invalid_opcode; // Set invalid opcode
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "WebSocket header is incomplete for extended payload length.");
                return header; // Return an empty header
            }
            header.payload_length = ((uint64_t)header_buffer_[2] << 56) |
                                    ((uint64_t)header_buffer_[3] << 48) |
                                    ((uint64_t)header_buffer_[4] << 40) |
                                    ((uint64_t)header_buffer_[5] << 32) |
                                    ((uint64_t)header_buffer_[6] << 24) |
                                    ((uint64_t)header_buffer_[7] << 16) |
                                    ((uint64_t)header_buffer_[8] << 8) |
				                    header_buffer_[9]; // Combine the eight bytes
            next_byte_index = 10; // Move to the next byte after payload length
            break;
		default: // Normal payload length (0-125)
            header.payload_length = header_buffer_[1] & 0x7F; // Mask to get the payload length only
    }
    if (header.is_masked) // Read masking key if present
    {
        if(head_size_ < next_byte_index + 4) // We need at least 4 bytes for the masking key
        {
			header.opcode = ws_opcode::invalid_opcode; // Set invalid opcode
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "WebSocket header is incomplete for masking key.");
            return header; // Return an empty header
		}
        header.mask_key[0] = header_buffer_[next_byte_index];
        header.mask_key[1] = header_buffer_[next_byte_index + 1];
        header.mask_key[2] = header_buffer_[next_byte_index + 2];
        header.mask_key[3] = header_buffer_[next_byte_index + 3];
    }
	return header; // Return the parsed header
}

rx_protocol_result_t rx_web_socket_endpoint::handle_ws_frame (io::rx_const_io_buffer buffer, size_t available)
{
    if (state_ != web_socket_state::payload_collect)
    {
        payload_to_read_ = current_header_.payload_length;
        auto buffer_result = port_->alloc_io_buffer();
        if (!buffer_result)
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to allocate buffer for WebSocket frame payload."s + buffer_result.errors_line());
            return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        receive_buffer_ = buffer_result.move_value();
    }
    size_t size = std::min(payload_to_read_, available);
    receive_buffer_.write(buffer.get_data(size), size);

    if (size == payload_to_read_)
    {// done with this packet

        if (current_header_.is_masked)
        {
            // Unmask the payload
            for (size_t i = 0; i < current_header_.payload_length; ++i)
            {
                receive_buffer_.buffer_ptr[i] ^= current_header_.mask_key[i % 4];
            }
        }
        switch (current_header_.opcode)
        {
            case ws_opcode::binary_frame:
            case ws_opcode::text_frame:
            case ws_opcode::continuation_frame:
                {
                    auto ret_val = data_collector_->handle_data_frame(current_header_, receive_buffer_);
                    if (ret_val != RX_PROTOCOL_OK)
                    {
                        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle new WebSocket frame."s + rx_protocol_error_message(ret_val));
                        return ret_val;
                    }
                }
                break;
            case ws_opcode::connection_close:
                {
                    auto ret_val = handle_close_frame(receive_buffer_, available);
                    if (ret_val != RX_PROTOCOL_OK)
                    {
                        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle new WebSocket frame."s + rx_protocol_error_message(ret_val));
                        return ret_val;
                    }
                }
                break;
            case ws_opcode::ping:
                {
                    auto ret_val = handle_ping_frame(receive_buffer_, available);
                    if (ret_val != RX_PROTOCOL_OK)
                    {
                        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle new WebSocket frame."s + rx_protocol_error_message(ret_val));
                        return ret_val;
                    }
                }
                break;
            case ws_opcode::pong:
                {
                    auto ret_val = handle_pong_frame(receive_buffer_, available);
                    if (ret_val != RX_PROTOCOL_OK)
                    {
                        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to handle new WebSocket frame."s + rx_protocol_error_message(ret_val));
                        return ret_val;
                    }
                }
                break;
            default:
                {
                    HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, rx_create_string("Unsupported WebSocket opcode received: ", (int)current_header_.opcode));
                    return RX_PROTOCOL_PARSING_ERROR;
                }
                break;
        }
        state_ = web_socket_state::idle;
        payload_to_read_ = 0;
        port_->release_io_buffer(std::move(receive_buffer_));
    }
	else if (payload_to_read_ > size)
    {
		payload_to_read_ -= size;
        state_ = web_socket_state::payload_collect;
    }
    else
    {
        RX_ASSERT(false);
        return RX_PROTOCOL_BUFFER_SIZE_ERROR;
    }
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_web_socket_endpoint::handle_close_frame (io::rx_io_buffer& buffer, size_t available)
{
    if (current_header_.payload_length == 2)
    {
        uint16_t code = (((uint16_t)receive_buffer_.buffer_ptr[0]) << 8) + receive_buffer_.buffer_ptr[1];
        HTTP_LOG_TRACE("rx_web_socket_endpoint", 500, rx_create_string("Client sent disconnect request with code ", code));
    }
    else if (current_header_.payload_length == 0)
    {
        HTTP_LOG_TRACE("rx_web_socket_endpoint", 500, rx_create_string("Client sent disconnect request without specific code"));
    }
    else
    {
        return RX_PROTOCOL_PARSING_ERROR;
    }
    uint8_t close_response[4] =
    { 0x88, 0x2, 0x03, 0xE8 }; // Normal closure code 1000

    auto buff = port_->alloc_io_buffer();
    if (buff)
    {
        buff.value().write(close_response, sizeof(close_response));
    }
    else
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to allocate buffer for WebSocket close response."s + buff.errors_line());
        return RX_PROTOCOL_OUT_OF_MEMORY;
    }

    auto close_packet = rx_create_send_packet(0, &buff.value(), 0, 0);
    auto send_result = rx_move_packet_down(&stack_entry, close_packet);
    port_->release_io_buffer(std::move(receive_buffer_));
    if (send_result != RX_PROTOCOL_OK)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to send WebSocket close response.");
        return send_result;
    }
    rx_close(&stack_entry, RX_PROTOCOL_OK);

    // Close the endpoint
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_web_socket_endpoint::handle_ping_frame (io::rx_io_buffer& buffer, size_t available)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED; // Ping/Pong handling is not yet implemented
}

rx_protocol_result_t rx_web_socket_endpoint::handle_pong_frame (io::rx_io_buffer& buffer, size_t available)
{
    return RX_PROTOCOL_OK; // Ping/Pong handling is not yet implemented
}


// Class protocols::rx_http::rx_web_socket_port 

rx_web_socket_port::rx_web_socket_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<rx_web_socket_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



rx_result rx_web_socket_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = rx_web_socket_port_base::initialize_runtime(ctx);
    if (result)
    {
        result = status.initialize(ctx);
    }
    return result;
}

rx_result rx_web_socket_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    auto result = rx_web_socket_port_base::deinitialize_runtime(ctx);

    return result;
}

void rx_web_socket_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (local_addr.is_null())
    {
        string_type str_addr;

        auto ascii = binder_data.get_value("Bind.Endpoint");
        if (!ascii.is_null() && ascii.is_string())
        {
            str_addr = ascii.get_string();
            io::string_address addr(str_addr);
            local_addr = &addr;
        }
    }
}

void rx_web_socket_port::stack_assembled ()
{
    listen(nullptr, nullptr);
}

rx_result rx_web_socket_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return true;
}


// Class protocols::rx_http::rx_web_socket_client_port 

rx_web_socket_client_port::rx_web_socket_client_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
        {
            auto rt = std::make_unique<rx_web_socket_client_endpoint>(this, path_);
            return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
        };
}


rx_web_socket_client_port::~rx_web_socket_client_port()
{
}



rx_result rx_web_socket_client_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    path_ = ctx.get_item_static("Connect.Endpoint", ""s);
    auto result = rx_web_socket_client_base::initialize_runtime(ctx);
    if (result)
    {
        result = status.initialize(ctx);
    }
    return result;
}

rx_result rx_web_socket_client_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}

void rx_web_socket_client_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (remote_addr.is_null())
    {
        string_type str_addr;

        auto ascii = binder_data.get_value("Connect.Endpoint");
        if (!ascii.is_null() && ascii.is_string())
        {
            str_addr = ascii.get_string();
            io::string_address addr(str_addr);
            remote_addr = &addr;
        }
    }
}


// Class protocols::rx_http::rx_web_socket_client_endpoint 

rx_web_socket_client_endpoint::rx_web_socket_client_endpoint (rx_web_socket_client_port* port, const string_type& path)
      : port_(port),
        executer_(-1),
        state_(web_socket_state::initial),
        head_size_(0),
        payload_to_read_(0),
        path_(path)
    , parser_(true)
{
    HTTP_LOG_DEBUG("rx_web_socket_endpoint", 200, "HTTP WS communication server endpoint created.");
    rx_init_stack_entry(&stack_entry, this);
    stack_entry.received_function = &rx_web_socket_client_endpoint::received_function;
    stack_entry.send_function = &rx_web_socket_client_endpoint::send_function;
    stack_entry.connected_function = &rx_web_socket_client_endpoint::connected_function;
    stack_entry.disconnected_function = &rx_web_socket_client_endpoint::disconnected_function;

    executer_ = port->get_executer();
    parser_.client_callback = [this](http_parsed_response resp) -> rx_protocol_result_t
        {
            return parse_http_response(std::move(resp));
        };
}


rx_web_socket_client_endpoint::~rx_web_socket_client_endpoint()
{
}



rx_protocol_result_t rx_web_socket_client_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
    rx_web_socket_client_endpoint* self = reinterpret_cast<rx_web_socket_client_endpoint*>(reference->user_data);
    self->port_->status.set_connected();
    if (self->state_ == web_socket_state::initial)
    {
		return self->connected(session);
    }
    else
    {
        return RX_PROTOCOL_INVALID_SEQUENCE; // Cannot send data before the handshake is complete
    }
}

rx_protocol_result_t rx_web_socket_client_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
    rx_web_socket_client_endpoint* self = reinterpret_cast<rx_web_socket_client_endpoint*>(reference->user_data);
    self->port_->status.set_disconnected();
    if (self->state_ != web_socket_state::initial)
        self->state_ = web_socket_state::initial;
    rx_notify_disconnected(&self->stack_entry, session, reason);
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_web_socket_client_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    rx_web_socket_client_endpoint* self = reinterpret_cast<rx_web_socket_client_endpoint*>(reference->user_data);
    self->port_->status.received_packet();
    if (self->state_ == web_socket_state::initial)
    {
        auto result = self->parser_.received_response(packet);
        if (result != RX_PROTOCOL_OK)
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to parse HTTP request for WebSocket Upgrade: "s
                + rx_protocol_error_message(result));
        }
        return result;
    }
    else
    {
        return self->received(packet);
    }
}

rx_protocol_result_t rx_web_socket_client_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    rx_web_socket_client_endpoint* self = reinterpret_cast<rx_web_socket_client_endpoint*>(reference->user_data);
    if (self->state_ == web_socket_state::initial)
    {
        return RX_PROTOCOL_INVALID_SEQUENCE; // Cannot send data before the handshake is complete
    }
    else
    {
        return self->send(packet);
    }
}

void rx_web_socket_client_endpoint::close_endpoint ()
{
}

rx_protocol_result_t rx_web_socket_client_endpoint::parse_http_response (http_parsed_response resp)
{
    if (resp.status == 101 && resp.message == "Switching Protocols")
    {
        // Check for required headers
        auto upgrade_it = resp.headers.find("Upgrade");
        auto connection_it = resp.headers.find("Connection");
        auto accept_it = resp.headers.find("Sec-WebSocket-Accept");
        if (upgrade_it != resp.headers.end() &&
            connection_it != resp.headers.end() &&
            accept_it != resp.headers.end())
        {
            if (upgrade_it->second != "websocket"
                || connection_it->second != "Upgrade"
                || accept_it->second.empty())
            {
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Missing required headers in WebSocket Upgrade response.");
                return RX_PROTOCOL_PARSING_ERROR;
            }
            else
            {
				accept_key_ = accept_it->second;
                state_ = web_socket_state::idle;
                auto session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);
				auto result = rx_notify_connected(&stack_entry, &session);
                if (result != RX_PROTOCOL_OK)
                {
                    HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to notify connected after WebSocket handshake.");
                    return result;
                }
				return result;
            }
        }
    }
	return RX_PROTOCOL_PARSING_ERROR;
}

string_type rx_web_socket_client_endpoint::create_websocket_key ()
{
	// Generate a random 16-byte value
    uint8_t random_bytes[16];
    auto result = rx_crypt_gen_random(random_bytes, sizeof(random_bytes));
    if (result != RX_OK)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to generate random bytes for WebSocket key.");
        return string_type();
    }
    string_value_struct str;
    result = rx_base64_get_string(&str, random_bytes, sizeof(random_bytes));
    if(result != RX_OK)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to generate WebSocket key.");
        return string_type();
	}
    return rx_c_str(&str);
}

rx_protocol_result_t rx_web_socket_client_endpoint::connected (rx_session* session)
{
    io::any_address remote_addr(session->remote_addr);
	string_type host_sr = remote_addr.to_string();

	auto buffer = port_->alloc_io_buffer();
    if(!buffer)
    {
        HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to allocate buffer for WebSocket handshake response."s + buffer.errors_line());
        return RX_PROTOCOL_OUT_OF_MEMORY;
	}

    buffer.value().write_chars(
        "GET /"+ path_ + " HTTP/1.1\r\n"
        "Host: " + host_sr + "\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: " + create_websocket_key() + "\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n"
	);

	auto result = rx_move_packet_down(&stack_entry, rx_create_send_packet(0, &buffer.value(), 0, 0));

    port_->release_io_buffer(buffer.move_value());

	return result;
}

rx_protocol_result_t rx_web_socket_client_endpoint::received (recv_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t rx_web_socket_client_endpoint::send (send_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

bool rx_web_socket_client_endpoint::is_header_done ()
{
    return false;
}

web_socket_header rx_web_socket_client_endpoint::parse_header ()
{
    return web_socket_header();
}


// Class protocols::rx_http::rx_web_socket_data_packet_collector 

rx_web_socket_data_packet_collector::rx_web_socket_data_packet_collector (rx_protocol_stack_endpoint* stack_entry)
      : receive_buffer_(0x1000, 0),
        collecting_(false),
        stack_entry_(stack_entry)
{
}



rx_protocol_result_t rx_web_socket_data_packet_collector::handle_data_frame (const web_socket_header& head, io::rx_io_buffer& buffer)
{
    if(!collecting_)
    {
        if(head.opcode != ws_opcode::text_frame && head.opcode != ws_opcode::binary_frame)
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Received non-initial WebSocket frame with invalid opcode.");
            return RX_PROTOCOL_PARSING_ERROR;
		}
        if(head.is_final)
        {
            rx_const_packet_buffer recv_buff{};
            rx_init_const_packet_buffer(&recv_buff, buffer.buffer_ptr, buffer.size);
            auto recv_pack = rx_create_recv_packet(0, &recv_buff, 0, 0);
            auto move_result = rx_move_packet_up(stack_entry_, recv_pack);
            if (move_result != RX_PROTOCOL_OK)
            {
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to move WebSocket frame up");
                return move_result;
            }
        }
        else
        {
            collecting_ = true; // Start collecting a new message
			receive_buffer_.write(buffer.buffer_ptr, buffer.size); // Write the first fragment to the buffer
        }
	}
    else
    {
        if (head.opcode != ws_opcode::continuation_frame)
        {
            HTTP_LOG_ERROR("rx_web_socket_endpoint", 400, "Received non-continuation WebSocket frame while collecting a fragmented message.");
            return RX_PROTOCOL_PARSING_ERROR;
        }
        receive_buffer_.write(buffer.buffer_ptr, buffer.size); // Write the fragment to the buffer

        if (head.is_final)
        {
            collecting_ = false; // Stop collecting the message
            rx_const_packet_buffer recv_buff{};
            rx_init_const_packet_buffer(&recv_buff, receive_buffer_.buffer_ptr, receive_buffer_.size);
            auto recv_pack = rx_create_recv_packet(0, &recv_buff, 0, 0);
            auto move_result = rx_move_packet_up(stack_entry_, recv_pack);
            if (move_result != RX_PROTOCOL_OK)
            {
                HTTP_LOG_ERROR("rx_web_socket_endpoint", 500, "Failed to move WebSocket frame up");
                return move_result;
            }
            receive_buffer_.reinit(); // Clear the buffer for the next message
        }
    }
    return RX_PROTOCOL_OK;
}


} // namespace rx_http
} // namespace protocols

