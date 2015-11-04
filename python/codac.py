# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.36
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _codac
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


CodacError = _codac.CodacError

version = _codac.version
match_filefilter = _codac.match_filefilter
class Cursor(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Cursor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Cursor, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _codac.new_Cursor(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _codac.delete_Cursor
    __del__ = lambda self : None;
    def __deepcopy__(self,memo):
        return _codac.Cursor___deepcopy__(self)


    def __copy__(self):
        return _codac.Cursor___deepcopy__(self)

Cursor_swigregister = _codac.Cursor_swigregister
Cursor_swigregister(Cursor)

string_to_time = _codac.string_to_time
cursor_read_int8_array = _codac.cursor_read_int8_array
cursor_read_uint8_array = _codac.cursor_read_uint8_array
cursor_read_int16_array = _codac.cursor_read_int16_array
cursor_read_uint16_array = _codac.cursor_read_uint16_array
cursor_read_int32_array = _codac.cursor_read_int32_array
cursor_read_uint32_array = _codac.cursor_read_uint32_array
cursor_read_int64_array = _codac.cursor_read_int64_array
cursor_read_uint64_array = _codac.cursor_read_uint64_array
cursor_read_float_array = _codac.cursor_read_float_array
cursor_read_double_array = _codac.cursor_read_double_array
cursor_read_char_array = _codac.cursor_read_char_array
cursor_read_complex_double_pair = _codac.cursor_read_complex_double_pair
cursor_read_complex_double_split_array = _codac.cursor_read_complex_double_split_array
cursor_read_complex_double_pairs_array = _codac.cursor_read_complex_double_pairs_array
cursor_read_complex = _codac.cursor_read_complex
cursor_read_complex_array = _codac.cursor_read_complex_array
cursor_read_bits = _codac.cursor_read_bits
cursor_read_bytes = _codac.cursor_read_bytes
done = _codac.done
NaN = _codac.NaN
PlusInf = _codac.PlusInf
MinInf = _codac.MinInf
type_get_format_name = _codac.type_get_format_name
type_get_class_name = _codac.type_get_class_name
type_get_native_type_name = _codac.type_get_native_type_name
type_get_special_type_name = _codac.type_get_special_type_name
c_index_to_fortran_index = _codac.c_index_to_fortran_index
get_option_bypass_special_types = _codac.get_option_bypass_special_types
get_option_perform_boundary_checks = _codac.get_option_perform_boundary_checks
get_option_perform_conversions = _codac.get_option_perform_conversions
get_option_use_fast_size_expressions = _codac.get_option_use_fast_size_expressions
get_option_use_mmap = _codac.get_option_use_mmap
isNaN = _codac.isNaN
isInf = _codac.isInf
isPlusInf = _codac.isPlusInf
isMinInf = _codac.isMinInf
CODA_MAX_NUM_DIMS = _codac.CODA_MAX_NUM_DIMS
CODA_ERROR_EXPRESSION = _codac.CODA_ERROR_EXPRESSION
coda_array_ordering_c = _codac.coda_array_ordering_c
coda_array_ordering_fortran = _codac.coda_array_ordering_fortran
coda_ffs_error = _codac.coda_ffs_error
coda_ffs_could_not_open_file = _codac.coda_ffs_could_not_open_file
coda_ffs_could_not_access_directory = _codac.coda_ffs_could_not_access_directory
coda_ffs_unsupported_file = _codac.coda_ffs_unsupported_file
coda_ffs_match = _codac.coda_ffs_match
coda_ffs_no_match = _codac.coda_ffs_no_match
coda_format_ascii = _codac.coda_format_ascii
coda_format_binary = _codac.coda_format_binary
coda_format_xml = _codac.coda_format_xml
coda_format_hdf4 = _codac.coda_format_hdf4
coda_format_hdf5 = _codac.coda_format_hdf5
coda_format_cdf = _codac.coda_format_cdf
coda_format_netcdf = _codac.coda_format_netcdf
coda_record_class = _codac.coda_record_class
coda_array_class = _codac.coda_array_class
coda_integer_class = _codac.coda_integer_class
coda_real_class = _codac.coda_real_class
coda_text_class = _codac.coda_text_class
coda_raw_class = _codac.coda_raw_class
coda_special_class = _codac.coda_special_class
coda_special_no_data = _codac.coda_special_no_data
coda_special_vsf_integer = _codac.coda_special_vsf_integer
coda_special_time = _codac.coda_special_time
coda_special_complex = _codac.coda_special_complex
coda_native_type_not_available = _codac.coda_native_type_not_available
coda_native_type_int8 = _codac.coda_native_type_int8
coda_native_type_uint8 = _codac.coda_native_type_uint8
coda_native_type_int16 = _codac.coda_native_type_int16
coda_native_type_uint16 = _codac.coda_native_type_uint16
coda_native_type_int32 = _codac.coda_native_type_int32
coda_native_type_uint32 = _codac.coda_native_type_uint32
coda_native_type_int64 = _codac.coda_native_type_int64
coda_native_type_uint64 = _codac.coda_native_type_uint64
coda_native_type_float = _codac.coda_native_type_float
coda_native_type_double = _codac.coda_native_type_double
coda_native_type_char = _codac.coda_native_type_char
coda_native_type_string = _codac.coda_native_type_string
coda_native_type_bytes = _codac.coda_native_type_bytes
init = _codac.init
set_option_bypass_special_types = _codac.set_option_bypass_special_types
set_option_perform_boundary_checks = _codac.set_option_perform_boundary_checks
set_option_perform_conversions = _codac.set_option_perform_conversions
set_option_use_fast_size_expressions = _codac.set_option_use_fast_size_expressions
set_option_use_mmap = _codac.set_option_use_mmap
datetime_to_double = _codac.datetime_to_double
double_to_datetime = _codac.double_to_datetime
coda_time_to_string = _codac.coda_time_to_string
recognize_file = _codac.recognize_file
open = _codac.open
close = _codac.close
get_product_filename = _codac.get_product_filename
get_product_file_size = _codac.get_product_file_size
get_product_format = _codac.get_product_format
get_product_class = _codac.get_product_class
get_product_type = _codac.get_product_type
get_product_version = _codac.get_product_version
get_product_root_type = _codac.get_product_root_type
get_product_variable_value = _codac.get_product_variable_value
type_has_ascii_content = _codac.type_has_ascii_content
type_get_format = _codac.type_get_format
type_get_class = _codac.type_get_class
type_get_read_type = _codac.type_get_read_type
type_get_string_length = _codac.type_get_string_length
type_get_bit_size = _codac.type_get_bit_size
type_get_name = _codac.type_get_name
type_get_description = _codac.type_get_description
type_get_unit = _codac.type_get_unit
type_get_fixed_value = _codac.type_get_fixed_value
type_get_num_record_fields = _codac.type_get_num_record_fields
type_get_record_field_index_from_name = _codac.type_get_record_field_index_from_name
type_get_record_field_type = _codac.type_get_record_field_type
type_get_record_field_name = _codac.type_get_record_field_name
type_get_record_field_hidden_status = _codac.type_get_record_field_hidden_status
type_get_record_field_available_status = _codac.type_get_record_field_available_status
type_get_record_union_status = _codac.type_get_record_union_status
type_get_array_num_dims = _codac.type_get_array_num_dims
type_get_array_dim = _codac.type_get_array_dim
type_get_array_base_type = _codac.type_get_array_base_type
type_get_special_type = _codac.type_get_special_type
type_get_special_base_type = _codac.type_get_special_base_type
cursor_set_product = _codac.cursor_set_product
cursor_goto_first_record_field = _codac.cursor_goto_first_record_field
cursor_goto_next_record_field = _codac.cursor_goto_next_record_field
cursor_goto_record_field_by_index = _codac.cursor_goto_record_field_by_index
cursor_goto_record_field_by_name = _codac.cursor_goto_record_field_by_name
cursor_goto_available_union_field = _codac.cursor_goto_available_union_field
cursor_goto_first_array_element = _codac.cursor_goto_first_array_element
cursor_goto_next_array_element = _codac.cursor_goto_next_array_element
cursor_goto_array_element = _codac.cursor_goto_array_element
cursor_goto_array_element_by_index = _codac.cursor_goto_array_element_by_index
cursor_goto_attributes = _codac.cursor_goto_attributes
cursor_goto_root = _codac.cursor_goto_root
cursor_goto_parent = _codac.cursor_goto_parent
cursor_use_base_type_of_special_type = _codac.cursor_use_base_type_of_special_type
cursor_has_ascii_content = _codac.cursor_has_ascii_content
cursor_get_string_length = _codac.cursor_get_string_length
cursor_get_bit_size = _codac.cursor_get_bit_size
cursor_get_byte_size = _codac.cursor_get_byte_size
cursor_get_num_elements = _codac.cursor_get_num_elements
cursor_get_product_file = _codac.cursor_get_product_file
cursor_get_depth = _codac.cursor_get_depth
cursor_get_index = _codac.cursor_get_index
cursor_get_file_bit_offset = _codac.cursor_get_file_bit_offset
cursor_get_file_byte_offset = _codac.cursor_get_file_byte_offset
cursor_get_format = _codac.cursor_get_format
cursor_get_type_class = _codac.cursor_get_type_class
cursor_get_read_type = _codac.cursor_get_read_type
cursor_get_special_type = _codac.cursor_get_special_type
cursor_get_type = _codac.cursor_get_type
cursor_get_record_field_index_from_name = _codac.cursor_get_record_field_index_from_name
cursor_get_record_field_available_status = _codac.cursor_get_record_field_available_status
cursor_get_available_union_field_index = _codac.cursor_get_available_union_field_index
cursor_get_array_dim = _codac.cursor_get_array_dim
cursor_read_int8 = _codac.cursor_read_int8
cursor_read_uint8 = _codac.cursor_read_uint8
cursor_read_int16 = _codac.cursor_read_int16
cursor_read_uint16 = _codac.cursor_read_uint16
cursor_read_int32 = _codac.cursor_read_int32
cursor_read_uint32 = _codac.cursor_read_uint32
cursor_read_int64 = _codac.cursor_read_int64
cursor_read_uint64 = _codac.cursor_read_uint64
cursor_read_float = _codac.cursor_read_float
cursor_read_double = _codac.cursor_read_double
cursor_read_char = _codac.cursor_read_char
cursor_read_string = _codac.cursor_read_string
cursor_read_complex_double_split = _codac.cursor_read_complex_double_split


