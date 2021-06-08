

/****************************************************************************
*
*  soft_logic\soft_plc.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef soft_plc_h
#define soft_plc_h 1




namespace sl_runtime {
class sl_program_holder;
class program_context;
namespace builders {
class module_chanels_factory;

} // namespace builders
} // namespace sl_runtime


using namespace sl_runtime::builders;

#define FLOAT_ADDR_MASK 0x40 // maska dali je float

#define MAIN_ADDR_MASK 0x80 // maska za dali je lokalno

#define	PURE_TYPE_MASK 0x0f // maska za tip
#define IS_MAIN(t) ((t&MAIN_ADDR_MASK)!=0)
#define IS_FLOAT(t) ((t&FLOAT_ADDR_MASK)!=0)
#define PURE_TYPE(t) ((adr_type)(t&PURE_TYPE_MASK))


namespace sl_runtime {

typedef uint32_t dword;
typedef uint8_t byte;


class initialize_context;
class deinitialize_context;
class sl_program;
class sl_scheme;
namespace instructions_runtime
{
	class subprogram_instruction;
}
enum adr_type
{
	input_bits=0,
	output_bits=1,
	memory_bits=2,
	input_registers=3,
	output_registers=4,
	memory_registers=5,
	system_registers=6,
	counter_registers=7,
	counter_bits=8,
	mono_registers=9,
	mono_bits=10,
	timer_registers=11,
	timer_bits=12,

	const_float_value=0xff,
	const_value=0xbf

};


class sl_instruction_base;
class ladder_program_context;
struct watch_addr_type
{
	adr_type type;
	uint32_t address;
};

struct schema_elem
{
	bool horizontal;
	bool vertical;
	bool active_horizontal_start;
	bool active_horizontal_end;
	bool active_vertical;
	bool active_element;
	sl_instruction_base* instruction;
};
typedef std::vector<schema_elem> col_type;
typedef std::vector<col_type> schema_type;





class sl_chanel_base 
{

  public:
      sl_chanel_base (int order);

      virtual ~sl_chanel_base();


      virtual void process_inputs (program_context* context, int module, const rx_time& now);

      virtual void process_outputs (program_context* context, int module, const rx_time& now);


  protected:

      void set_input_bit (int num, int module, bool val, program_context* context);

      bool get_output_bit (int num, int module, program_context* context);

      uint32_t get_output_register (int num, int module, program_context* context);

      void set_input_register (int num, int module, uint32_t val, program_context* context);

      float get_output_float (int num, int module, program_context* context);

      void set_input_float (int num, int module, float val, program_context* context);

      bool get_out_bit_written (int num, int module, program_context* context);

      bool get_out_register_written (int num, int module, program_context* context);


      const int get_order () const
      {
        return order_;
      }



  private:
      sl_chanel_base(const sl_chanel_base &right);

      sl_chanel_base & operator=(const sl_chanel_base &right);



      int order_;


};







class sl_module_base 
{
	typedef std::vector<sl_chanel_base*> chanels_type;
	typedef std::vector<sl_chanel_base*>::iterator chanels_iterator;

  public:
      sl_module_base (int order);

      virtual ~sl_module_base();


      virtual void process_inputs (program_context* context, const rx_time& now);

      virtual void process_outputs (program_context* context, const rx_time& now);

      virtual void create_chanels (module_chanels_factory* builder) = 0;


      const int get_order () const
      {
        return order_;
      }



  protected:

      chanels_type chanels_;


  private:
      sl_module_base(const sl_module_base &right);

      sl_module_base & operator=(const sl_module_base &right);



      int order_;


};






class sl_di_chanel : public sl_chanel_base  
{

  public:
      sl_di_chanel (int order);

      ~sl_di_chanel();


  protected:

      void set_input_bit (bool value, int module, program_context* context);

      void set_input_status (bool status, int module, program_context* context);


  private:


};







class sl_di_module : public sl_module_base  
{

  public:
      sl_di_module (int order);

      ~sl_di_module();


      void create_chanels (module_chanels_factory* builder);


  protected:

  private:


};



//	bazna klasa za klase koje obezbedjuju kontekst za
//	izvrsenje programa



class program_executer 
{

  public:
      program_executer (sl_program_holder* program);

      ~program_executer();


      void program_scan ();

      virtual void start_program (uint32_t rate) = 0;

      virtual void stop_program () = 0;

      virtual void delete_executer () = 0;


  protected:

  private:


      sl_program_holder *program_;


};






class address_space 
{
	typedef std::map<uint32_t,uint32_t> address_range_type;
	typedef std::map<uint32_t,uint32_t>::iterator address_range_iterator;
	typedef std::map<adr_type,address_range_type> address_space_type;
	typedef std::map<adr_type,address_range_type>::iterator address_space_iterator;

  public:
      address_space();

      virtual ~address_space();


      bool get_bit (adr_type type, dword reg, byte bit);

      void set_bit (adr_type type, dword reg, byte bit, bool val);

      bool get_positive_transact (adr_type type, dword reg, byte bit);

      bool get_negative_transact (adr_type type, dword reg, byte bit);

      void init_scan ();

      dword get_register (adr_type type, dword reg);

      void set_register (adr_type type, dword reg, dword val);

      float get_float (adr_type type, dword reg);

      void set_float (adr_type type, dword reg, float val);

      bool get_out_bit_written (dword reg, byte bit);

      bool get_out_register_written (dword reg);


  protected:

  private:
      address_space(const address_space &right);

      address_space & operator=(const address_space &right);


      void init_prevs (adr_type type);

      void init_changes (adr_type type);



      address_space_type values_;

      address_space_type prev_values_;

      address_space_type changed_values_;

      address_space_type forced_bits_;

      address_space_type forced_mask_;

	  uint32_t and_masks[0x20];
	  uint32_t or_masks[0x20];
};






class sl_do_chanel : public sl_chanel_base  
{

  public:
      sl_do_chanel (int order);

      ~sl_do_chanel();


  protected:

      bool get_output_bit (int module, program_context* context);

      void set_done_bit (bool value, int module, program_context* context);

      void set_error_status (bool status, int module, program_context* context);

      bool get_out_bit_written (int module, program_context* context);


  private:


};







class sl_do_module : public sl_module_base  
{

  public:
      sl_do_module (int order);

      ~sl_do_module();


      void create_chanels (module_chanels_factory* builder);


  protected:

  private:


};



//	pomocna klasa koja obezbedjuje apstrakciju registra ili
//	vrednosti za operacije



class argument_value 
{

  public:
      argument_value();

      argument_value(const argument_value &right);

      argument_value (float value);

      argument_value (int value);

      argument_value (adr_type type, dword addr);

      ~argument_value();

      argument_value & operator=(const argument_value &right);


      int get_value (program_context* context);

      float get_float (program_context* context);

      bool is_float ();

      double get_double (program_context* context);


  protected:

  private:


      adr_type type_;

      dword addr_;

      int value_;


};






class register_value 
{

  public:
      register_value();

      register_value(const register_value &right);

      register_value (adr_type type, dword addr);

      ~register_value();

      register_value & operator=(const register_value &right);


      dword get_value (program_context* context);

      void set_value (program_context* context, dword val);

      uint64_t make_cache_value ();

      float get_float (program_context* context);

      void set_float (program_context* context, float val);

      bool is_float ();

      double get_double (program_context* context);

      void set_double (program_context* context, double val);


      const adr_type get_type () const
      {
        return type_;
      }


      const dword get_addr () const
      {
        return addr_;
      }



  protected:

  private:


      adr_type type_;

      dword addr_;


};






class bit_value 
{

  public:
      bit_value();

      bit_value(const bit_value &right);

      bit_value (adr_type type, dword addr, byte bit);

      ~bit_value();

      bit_value & operator=(const bit_value &right);


      bool get_value (program_context* context) const;

      void set_value (program_context* context, bool val);

      adr_type get_type () const;

      dword get_addr () const;

      byte get_bit () const;

      bool get_positive_transact (program_context* context) const;

      bool get_negative_transact (program_context* context) const;


  protected:

      adr_type type_;

      dword addr_;

      byte bit_;


  private:


};






class sl_ai_chanel : public sl_chanel_base  
{

  public:
      sl_ai_chanel (int order);

      ~sl_ai_chanel();


  protected:

      void set_input_reg (dword value, int module, program_context* context);

      void set_input_status (bool status, int module, program_context* context);

      void set_input_float (float value, int module, program_context* context);


  private:


};







class sl_ai_module : public sl_module_base  
{

  public:
      sl_ai_module (int order);

      ~sl_ai_module();


      void create_chanels (module_chanels_factory* builder);


  protected:

  private:


};






class sl_ao_chanel : public sl_chanel_base  
{

  public:
      sl_ao_chanel (int order);

      ~sl_ao_chanel();


  protected:

      dword get_output_register (int module, program_context* context);

      float get_output_float (int module, program_context* context);

      bool get_out_register_written (int module, program_context* context);

      void set_done_bit (bool value, int module, program_context* context);

      void set_error_status (bool status, int module, program_context* context);


  private:


};







class sl_ao_module : public sl_module_base  
{

  public:
      sl_ao_module (int order);

      ~sl_ao_module();


      void create_chanels (module_chanels_factory* builder);


  protected:

  private:


};






class watch_base 
{

  public:
      watch_base (program_context* context);

      virtual ~watch_base();


      virtual void set_debug_data (int wnd, const std::vector<dword>& data, const rx_time& now) = 0;


  protected:

      program_context* get_context ();


  private:
      watch_base(const watch_base &right);

      watch_base & operator=(const watch_base &right);



      program_context *context_;


};






class debug_segment_base 
{

  public:
      debug_segment_base (program_context* context);

      ~debug_segment_base();


      virtual void set_debug_data (const rx_uuid& guid, const std::vector<bool>& data, const rx_time& now, int segment) = 0;


  protected:

      program_context* get_context ();


  private:


      program_context *context_;


};






class program_context 
{
	typedef std::vector<watch_addr_type> watches_type;
	typedef std::vector<watch_addr_type>::iterator watches_iterator;

	//typedef std::map<sl_instruction_base*,server::pinable*> registred_jobs_type;
	//typedef std::map<sl_instruction_base*,server::pinable*>::iterator registred_jobs_iterator;

  public:
      program_context (program_context* parent, sl_program_holder* holder);

      virtual ~program_context();


      virtual void initialize (initialize_context* ctx);

      virtual void deinitialize (deinitialize_context* ctx);

      bool get_bit (adr_type type, dword reg, byte bit);

      void set_bit (adr_type type, dword reg, byte bit, bool val);

      bool get_positive_transact (adr_type type, dword reg, byte bit);

      bool get_negative_transact (adr_type type, dword reg, byte bit);

      virtual void init_scan ();

      dword get_register (adr_type type, dword reg);

      void set_register (adr_type type, dword reg, dword val);

      void set_watch_window (int wnd_id, const std::vector<watch_addr_type>& data);

      void process_watches (const rx_time& now);

      sl_program_holder* get_program_holder ();

      virtual void create_debug_data (sl_program_holder* holder);

      bool set_debug_segment (int segment, const rx_uuid& guid);

      void get_debug_segment (int& segment);

      void set_debug_data (const std::vector<bool>& data, const rx_time& now, int segment);

      float get_float (adr_type type, dword reg);

      void set_float (adr_type type, dword reg, float val);

      bool get_out_bit_written (dword reg, byte bit);

      bool get_out_register_written (dword reg);


      const rx_uuid& get_guid () const
      {
        return guid_;
      }

      void set_guid (const rx_uuid& value)
      {
        guid_ = value;
      }



  protected:

      program_context *parent_;


  private:
      program_context(const program_context &right);

      program_context & operator=(const program_context &right);


      address_space* get_address_space (adr_type type);

      void set_debug_data (const std::vector<bool>& data, const rx_time& now, int segment, const rx_uuid& guid);

      void process_watches (const rx_time& now, program_context* read_from);



      address_space addresses_;

      watch_base *watch_base_;

      sl_program_holder *holder_;

      debug_segment_base *dbg_segment_;


      watches_type watches_;

      int wnd_id_;

      watches_type temp_watches_;

      int temp_wnd_id_;

      rx_uuid guid_;

      int debug_segment_;


};






class ladder_program_context : public program_context  
{

  public:
      ladder_program_context (program_context* parent, sl_program_holder* holder);

      ~ladder_program_context();


      void initialize (initialize_context* ctx);

      void deinitialize (deinitialize_context* ctx);


  protected:

  private:


};






class initialize_context 
{

  public:
      initialize_context();

      virtual ~initialize_context();


      virtual debug_segment_base* create_segment_debugger (program_context* context) = 0;

      virtual watch_base* create_watch_debugger (program_context* context) = 0;


  protected:

  private:
      initialize_context(const initialize_context &right);

      initialize_context & operator=(const initialize_context &right);



};






class custom_program_step 
{

  public:
      custom_program_step();

      custom_program_step(const custom_program_step &right);

      virtual ~custom_program_step();

      custom_program_step & operator=(const custom_program_step &right);


      virtual void process_step_before (program_context* context, const rx_time& now);

      virtual void process_step_after (program_context* context, const rx_time& now);

      virtual void initialize (sl_program_holder* holder, initialize_context* ctx);

      virtual void deinitialize ();


  protected:

  private:


};






class deinitialize_context 
{

  public:
      deinitialize_context();

      ~deinitialize_context();


  protected:

  private:


};






class io_program_data 
{

  public:
      io_program_data();

      ~io_program_data();


      virtual bool get_input (int idx) = 0;

      virtual void set_output (int idx, bool val) = 0;

      virtual void set_instruction_active (bool val) = 0;


  protected:

  private:


};







class sl_program 
{

  public:
      sl_program();

      sl_program(const sl_program &right);

      virtual ~sl_program();

      sl_program & operator=(const sl_program &right);


      virtual void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      virtual void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      virtual void process_program (program_context* context, const rx_time& now, bool debug);

      virtual program_context* create_program_context (program_context* parent_context, sl_program_holder* holder) = 0;

      virtual void load (FILE* file, dword version);


  protected:

  private:


};







class sl_instruction_base 
{

  public:
      sl_instruction_base();

      virtual ~sl_instruction_base();


      virtual void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now) = 0;

      virtual void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      virtual void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      virtual int get_inputs () = 0;

      virtual int get_outputs () = 0;

      virtual int get_ladder_size () = 0;


  protected:

  private:
      sl_instruction_base(const sl_instruction_base &right);

      sl_instruction_base & operator=(const sl_instruction_base &right);



};







class sl_scheme 
{

  public:
      sl_scheme (int width, int height);

      virtual ~sl_scheme();


      void process_schema (ladder_program_context* context, const rx_time& now);

      virtual void load (FILE* file, dword version);

      void get_debug_data (std::vector<bool>& data);

      void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      void set_vertical (int x, int y);

      void set_horizontal (int x, int y);

      void set_instruction (int x, int y, sl_instruction_base* inst);


  protected:

  private:
      sl_scheme(const sl_scheme &right);

      sl_scheme & operator=(const sl_scheme &right);


      void init_runtime ();



      schema_type runtime_schema_;

      int width_;

      int height_;


};






class sl_ladder_program : public sl_program  
{
	typedef std::list<sl_scheme*> schemas_type;
	typedef std::list<sl_scheme*>::iterator schemas_iterator;

  public:
      sl_ladder_program();

      ~sl_ladder_program();


      void process_program (program_context* context, const rx_time& now, bool debug);

      program_context* create_program_context (program_context* parent_context, sl_program_holder* holder);

      void load (FILE* file, dword version);

      void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      void add_schema (sl_scheme* sch);


  protected:

  private:


      schemas_type schemas_;


};


struct subcontext_data
{
	sl_program* program;
	program_context* context;
};








class sl_program_holder 
{
	typedef std::list<sl_module_base*> modules_type;
	typedef std::list<sl_module_base*>::iterator modules_iterator;
	typedef std::map<string_type,sl_program*> subprograms_type;
	typedef std::map<string_type,sl_program*>::iterator subprograms_iterator;
	typedef std::map<sl_instruction_base*,subcontext_data> subcontexts_type;
	typedef std::map<sl_instruction_base*,subcontext_data>::iterator subcontexts_iterator;
	typedef std::map<rx::rx_uuid,subcontext_data> subcontexts_hash;
	typedef std::map<rx::rx_uuid,subcontext_data>::iterator subcontexts_hash_iterator;
	typedef std::vector<custom_program_step*> custom_steps_type;
	typedef std::vector<custom_program_step*>::iterator custom_steps_iterator;

  public:
      sl_program_holder();

      virtual ~sl_program_holder();


      void initialize (initialize_context* ctx);

      void deinitialize (deinitialize_context* ctx);

      virtual void program_scan ();

      virtual void process_inputs (program_context* context, const rx_time& now);

      virtual void process_outputs (program_context* context, const rx_time& now);

      virtual void process_program (program_context* context, const rx_time& now, bool debug);

      virtual void load (FILE* file, dword version);

      virtual void start_program (program_executer* executer);

      virtual void stop_program ();

      bool register_subcontext (instructions_runtime::subprogram_instruction* inst, const string_type& name, initialize_context* ctx, program_context* current_context);

      program_context* get_subcontext (sl_instruction_base* inst);

      bool execute_sub (sl_instruction_base* inst, const rx_time& now);

      program_context* get_subcontext (const rx_uuid& guid);

      void set_debug_context (program_context* context);

      void set_main_program (sl_program* main, program_context* context = nullptr);

      void add_sub_program (const string_type& name, sl_program* prog);

      void add_custom_step (custom_program_step* step);

      void add_module (sl_module_base* module);

      program_context* get_main_context ();

      void deinitialize_sub (sl_instruction_base* inst, deinitialize_context* ctx);


  protected:

  private:
      sl_program_holder(const sl_program_holder &right);

      sl_program_holder & operator=(const sl_program_holder &right);



      program_executer *executer_;

      modules_type modules_;

      sl_program *main_;

      program_context *main_context_;

      subprograms_type subprograms_;

      custom_steps_type custom_steps_;


      dword rate_;

      string_type name_;

      subcontexts_type subcontexts_;

      program_context* debug_context_;

      subcontexts_hash sub_hash_;

      bool first_scan_;


};






class sl_programs_collection 
{
	typedef std::list<sl_program_holder*> programs_type;
	typedef std::list<sl_program_holder*>::iterator programs_iterator;

  public:
      sl_programs_collection();

      virtual ~sl_programs_collection();


      void deinitialize (deinitialize_context* ctx);

      void start_programs ();

      void stop_programs ();

      void add_program (sl_program_holder* prog);


  protected:

      virtual program_executer* get_executer (sl_program_holder* holder);


  private:
      sl_programs_collection(const sl_programs_collection &right);

      sl_programs_collection & operator=(const sl_programs_collection &right);



      programs_type programs_;


};






class laddder_io_program_data : public io_program_data  
{

  public:
      laddder_io_program_data (schema_type& schema, int& row, int& col, int inputs, int outputs);

      ~laddder_io_program_data();


      bool get_input (int idx);

      void set_output (int idx, bool val);

      void set_instruction_active (bool val);


  protected:

  private:


      schema_type& schema_;

      int row_;

      int col_;

      int inputs_;

      int outputs_;


};


namespace builders {





class module_chanels_factory 
{

  public:
      virtual ~module_chanels_factory();


      virtual sl_ai_chanel* create_ai_chanel (int order) = 0;

      virtual sl_di_chanel* create_di_chanel (int order) = 0;

      virtual sl_ao_chanel* create_ao_chanel (int order) = 0;

      virtual sl_do_chanel* create_do_chanel (int order) = 0;


  protected:
      module_chanels_factory();


  private:
      module_chanels_factory(const module_chanels_factory &right);

      module_chanels_factory & operator=(const module_chanels_factory &right);



};






class program_builder 
{

  public:
      virtual ~program_builder();


      virtual sl_program* build_program () = 0;


  protected:
      program_builder();


  private:
      program_builder(const program_builder &right);

      program_builder & operator=(const program_builder &right);



};







class program_holder_bulder 
{

  public:
      virtual ~program_holder_bulder();


      sl_program_holder* build_program_holder ();


  protected:
      program_holder_bulder (module_chanels_factory* chanels_factory);


      virtual sl_module_base* get_next_module (int order) = 0;

      virtual program_builder* get_main_program_builder () = 0;

      virtual program_builder* get_sub_program_builder (const string_type& name) = 0;


  private:
      program_holder_bulder(const program_holder_bulder &right);

      program_holder_bulder & operator=(const program_holder_bulder &right);



      module_chanels_factory *chanels_factory_;


};






class ladder_program_builder : public program_builder  
{

  public:
      ~ladder_program_builder();


      sl_program* build_program ();


  protected:
      ladder_program_builder();


      virtual sl_scheme* get_next_schema () = 0;


  private:


};


} // namespace builders
} // namespace sl_runtime



#endif
