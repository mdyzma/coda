/*
 * Copyright (C) 2007-2010 S[&]T, The Netherlands.
 *
 * This file is part of CODA.
 *
 * CODA is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CODA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CODA; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef CODA_HDF5_INTERNAL_H
#define CODA_HDF5_INTERNAL_H

#include "coda-hdf5.h"

/* HDF5 creates its own versions of uint32_t, int64_t, and uint64_t as typedefs */
/* We therefore disable our #define entries for these types if we have them */
#undef int64_t
#undef uint32_t
#undef uint64_t

#include "hdf5.h"

typedef enum hdf5_type_tag_enum
{
    tag_hdf5_basic_datatype,    /* coda_integer_class, coda_real_class, coda_text_class */
    tag_hdf5_compound_datatype, /* coda_record_class */
    tag_hdf5_attribute, /* coda_array_class */
    tag_hdf5_attribute_record,  /* coda_record_class */
    tag_hdf5_group,     /* coda_record_class */
    tag_hdf5_dataset    /* coda_array_class */
} hdf5_type_tag;

/* Inheritance tree:
 * coda_Type
 * \ -- coda_hdf5Type
 *      \ -- coda_hdf5DataType
 *           \ -- coda_hdf5BasicDataType
 *            |-- coda_hdf5CompoundDataType
 *       |-- coda_hdf5Attribute
 *       |-- coda_hdf5AttributeRecord
 *       |-- coda_hdf5Object
 *           \ -- coda_hdf5Group
 *            |-- coda_hdf5DataSet
 */

typedef struct coda_hdf5Type_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
} coda_hdf5Type;

typedef struct coda_hdf5Object_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    unsigned long fileno[2];
    unsigned long objno[2];
} coda_hdf5Object;

typedef struct coda_hdf5DataType_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    hid_t datatype_id;
} coda_hdf5DataType;

typedef struct coda_hdf5BasicDataType_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    hid_t datatype_id;
    coda_native_type read_type;
    int is_variable_string;
} coda_hdf5BasicDataType;

typedef struct coda_hdf5CompoundDataType_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    hid_t datatype_id;
    int num_members;
    coda_hdf5DataType **member;
    char **member_name;
    hid_t *member_type;
    hashtable *hash_data;
} coda_hdf5CompoundDataType;

typedef struct coda_hdf5Attribute_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    hid_t attribute_id;
    hid_t dataspace_id;
    hsize_t num_elements;
    int ndims;
    hsize_t dims[CODA_MAX_NUM_DIMS];
    coda_hdf5DataType *base_type;
} coda_hdf5Attribute;

typedef struct coda_hdf5AttributeRecord_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    hid_t obj_id;       /* id of object to which the attributes are attached */
    int num_attributes;
    coda_hdf5Attribute **attribute;
    char **attribute_name;
    hashtable *hash_data;
} coda_hdf5AttributeRecord;

typedef struct coda_hdf5Group_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    unsigned long fileno[2];
    unsigned long objno[2];
    hid_t group_id;
    hsize_t num_objects;
    struct coda_hdf5Object_struct **object;
    char **object_name;
    hashtable *hash_data;
    struct coda_hdf5AttributeRecord_struct *attributes;
} coda_hdf5Group;

typedef struct coda_hdf5DataSet_struct
{
    int retain_count;
    coda_format format;
    coda_type_class type_class;
    char *name;
    char *description;

    hdf5_type_tag tag;
    unsigned long fileno[2];
    unsigned long objno[2];
    hid_t dataset_id;
    hid_t dataspace_id;
    hsize_t num_elements;
    int ndims;
    hsize_t dims[CODA_MAX_NUM_DIMS];
    coda_hdf5DataType *base_type;
    struct coda_hdf5AttributeRecord_struct *attributes;
} coda_hdf5DataSet;

coda_hdf5AttributeRecord *coda_hdf5_empty_attribute_record();

struct coda_hdf5ProductFile_struct
{
    /* general fields (shared between all supported product types) */
    char *filename;
    int64_t file_size;
    coda_format format;
    coda_DynamicType *root_type;
    coda_ProductDefinition *product_definition;
    long *product_variable_size;
    int64_t **product_variable;

    hid_t file_id;
    hsize_t num_objects;
    struct coda_hdf5Object_struct **object;
};
typedef struct coda_hdf5ProductFile_struct coda_hdf5ProductFile;

#endif
