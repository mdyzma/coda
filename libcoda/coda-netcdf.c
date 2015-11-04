/*
 * Copyright (C) 2007-2011 S[&]T, The Netherlands.
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

#include "coda-netcdf-internal.h"
#include "coda-mem-internal.h"

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef WORDS_BIGENDIAN
static void swap_int16(int16_t *value)
{
    union
    {
        uint8_t as_bytes[2];
        int16_t as_int16;
    } v;

    v.as_bytes[0] = ((uint8_t *)value)[1];
    v.as_bytes[1] = ((uint8_t *)value)[0];

    *value = v.as_int16;
}

static void swap_int32(int32_t *value)
{
    union
    {
        uint8_t as_bytes[4];
        int32_t as_int32;
    } v;

    v.as_bytes[0] = ((uint8_t *)value)[3];
    v.as_bytes[1] = ((uint8_t *)value)[2];
    v.as_bytes[2] = ((uint8_t *)value)[1];
    v.as_bytes[3] = ((uint8_t *)value)[0];

    *value = v.as_int32;
}

static void swap_int64(int64_t *value)
{
    union
    {
        uint8_t as_bytes[8];
        int64_t as_int64;
    } v;

    v.as_bytes[0] = ((uint8_t *)value)[7];
    v.as_bytes[1] = ((uint8_t *)value)[6];
    v.as_bytes[2] = ((uint8_t *)value)[5];
    v.as_bytes[3] = ((uint8_t *)value)[4];
    v.as_bytes[4] = ((uint8_t *)value)[3];
    v.as_bytes[5] = ((uint8_t *)value)[2];
    v.as_bytes[6] = ((uint8_t *)value)[1];
    v.as_bytes[7] = ((uint8_t *)value)[0];

    *value = v.as_int64;
}

static void swap_float(float *value)
{
    union
    {
        uint8_t as_bytes[4];
        float as_float;
    } v;

    v.as_bytes[0] = ((uint8_t *)value)[3];
    v.as_bytes[1] = ((uint8_t *)value)[2];
    v.as_bytes[2] = ((uint8_t *)value)[1];
    v.as_bytes[3] = ((uint8_t *)value)[0];

    *value = v.as_float;
}

static void swap_double(double *value)
{
    union
    {
        uint8_t as_bytes[8];
        double as_double;
    } v;

    v.as_bytes[0] = ((uint8_t *)value)[7];
    v.as_bytes[1] = ((uint8_t *)value)[6];
    v.as_bytes[2] = ((uint8_t *)value)[5];
    v.as_bytes[3] = ((uint8_t *)value)[4];
    v.as_bytes[4] = ((uint8_t *)value)[3];
    v.as_bytes[5] = ((uint8_t *)value)[2];
    v.as_bytes[6] = ((uint8_t *)value)[1];
    v.as_bytes[7] = ((uint8_t *)value)[0];

    *value = v.as_double;
}
#endif

static int read_dim_array(coda_netcdf_product *product, int32_t num_records, int32_t *num_dims, int32_t **dim_length,
                          int *appendable_dim)
{
    int32_t tag;
    long i;

    if (read(product->fd, &tag, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&tag);
#endif

    if (read(product->fd, num_dims, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(num_dims);
#endif

    if (tag == 0)
    {
        if (*num_dims != 0)
        {
            coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for nelems for empty dim_array) "
                           "for file %s", product->filename);
            return -1;
        }
        return 0;
    }

    if (tag != 10)      /* NC_DIMENSION */
    {
        coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for NC_DIMENSION tag) for file %s",
                       product->filename);
        return -1;
    }

    *dim_length = malloc((*num_dims) * sizeof(int32_t));
    if (*dim_length == NULL)
    {
        coda_set_error(CODA_ERROR_OUT_OF_MEMORY, "out of memory (could not allocate %lu bytes) (%s:%u)",
                       (*num_dims) * sizeof(int32_t), __FILE__, __LINE__);
        return -1;
    }

    for (i = 0; i < *num_dims; i++)
    {
        int32_t string_length;

        /* nelems */
        if (read(product->fd, &string_length, 4) < 0)
        {
            free(*dim_length);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&string_length);
#endif
        /* skip chars + padding */
        if ((string_length & 3) != 0)
        {
            string_length += 4 - (string_length & 3);
        }
        if (lseek(product->fd, string_length, SEEK_CUR) < 0)
        {
            free(*dim_length);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
        /* dim_length */
        if (read(product->fd, &(*dim_length)[i], 4) < 0)
        {
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&(*dim_length)[i]);
#endif
        if ((*dim_length)[i] == 0)
        {
            /* appendable dimension */
            (*dim_length)[i] = num_records;
            *appendable_dim = i;
        }
    }

    return 0;
}

static int read_att_array(coda_netcdf_product *product, coda_mem_record **attributes, coda_conversion *conversion)
{
    coda_type_record *attributes_definition;
    int32_t tag;
    int32_t num_att;
    long i;

    if (read(product->fd, &tag, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&tag);
#endif

    if (read(product->fd, &num_att, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&num_att);
#endif

    if (tag == 0)
    {
        *attributes = NULL;
        if (num_att != 0)
        {
            coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for nelems for empty att_array) "
                           "for file %s", product->filename);
            return -1;
        }
        return 0;
    }

    if (tag != 12)      /* NC_ATTRIBUTE */
    {
        coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for NC_ATTRIBUTE tag) for file %s",
                       product->filename);
        return -1;
    }

    attributes_definition = coda_type_record_new(coda_format_netcdf);
    if (attributes_definition == NULL)
    {
        return -1;
    }
    *attributes = coda_mem_record_new(attributes_definition);
    coda_type_release((coda_type *)attributes_definition);
    if (*attributes == NULL)
    {
        return -1;
    }

    for (i = 0; i < num_att; i++)
    {
        coda_netcdf_basic_type *basic_type;
        int32_t string_length;
        int64_t offset;
        int32_t nc_type;
        int32_t nelems;
        long value_length;
        char *name;

        /* nelems */
        if (read(product->fd, &string_length, 4) < 0)
        {
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&string_length);
#endif
        /* chars */
        name = malloc(string_length + 1);
        if (name == NULL)
        {
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_OUT_OF_MEMORY, "out of memory (could not allocate %lu bytes) (%s:%u)",
                           (long)(string_length + 1), __FILE__, __LINE__);
            return -1;
        }
        name[string_length] = '\0';
        if (read(product->fd, name, string_length) < 0)
        {
            free(name);
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
        /* chars padding */
        if ((string_length & 3) != 0)
        {
            if (lseek(product->fd, 4 - (string_length & 3), SEEK_CUR) < 0)
            {
                free(name);
                coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                               strerror(errno));
                return -1;
            }
        }
        /* nc_type */
        if (read(product->fd, &nc_type, 4) < 0)
        {
            free(name);
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&nc_type);
#endif
        /* nelems */
        if (read(product->fd, &nelems, 4) < 0)
        {
            free(name);
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&nelems);
#endif
        value_length = nelems;
        switch (nc_type)
        {
            case 1:
            case 2:
                break;
            case 3:
                value_length *= 2;
                break;
            case 4:
            case 5:
                value_length *= 4;
                break;
            case 6:
                value_length *= 8;
                break;
            default:
                free(name);
                coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid netcdf type (%d)) for file %s",
                               (int)nc_type, product->filename);
                return -1;
        }
        if ((value_length & 3) != 0)
        {
            value_length += 4 - (value_length & 3);
        }
        offset = lseek(product->fd, value_length, SEEK_CUR);
        if (offset < 0)
        {
            free(name);
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
        offset -= value_length;

        if (conversion != NULL)
        {
            if (nelems == 1 && (strcmp(name, "scale_factor") == 0 || strcmp(name, "add_offset") == 0))
            {
                if (nc_type == 5)
                {
                    float value;

                    if (lseek(product->fd, offset, SEEK_SET) < 0)
                    {
                        free(name);
                        coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                       strerror(errno));
                        return -1;
                    }
                    if (read(product->fd, &value, 4) < 0)
                    {
                        free(name);
                        coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                       strerror(errno));
                        return -1;
                    }
#ifndef WORDS_BIGENDIAN
                    swap_float(&value);
#endif
                    if (name[0] == 's')
                    {
                        conversion->numerator = value;
                    }
                    else
                    {
                        conversion->add_offset = value;
                    }
                }
                else if (nc_type == 6)
                {
                    double value;

                    if (lseek(product->fd, offset, SEEK_SET) < 0)
                    {
                        free(name);
                        coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                       strerror(errno));
                        return -1;
                    }
                    if (read(product->fd, &value, 8) < 0)
                    {
                        free(name);
                        coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                       strerror(errno));
                        return -1;
                    }
#ifndef WORDS_BIGENDIAN
                    swap_double(&value);
#endif
                    if (name[0] == 's')
                    {
                        conversion->numerator = value;
                    }
                    else
                    {
                        conversion->add_offset = value;
                    }
                }
            }
            /* note that missing_value has preference over _FillValue */
            /* We therefore don't modify fill_value for _FillValue if fill_value was already set */
            if (nelems == 1 && nc_type != 2 && (strcmp(name, "missing_value") == 0 ||
                                                (strcmp(name, "_FillValue") == 0 &&
                                                 coda_isNaN(conversion->invalid_value))))
            {
                union
                {
                    int8_t as_int8[8];
                    int16_t as_int16[4];
                    int32_t as_int32[2];
                    int64_t as_int64[1];
                    float as_float[2];
                    double as_double[1];
                } value;

                if (lseek(product->fd, offset, SEEK_SET) < 0)
                {
                    free(name);
                    coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                    coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                   strerror(errno));
                    return -1;
                }
                if (read(product->fd, value.as_int8, value_length) < 0)
                {
                    free(name);
                    coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                    coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                                   strerror(errno));
                    return -1;
                }
                switch (nc_type)
                {
                    case 1:
                        conversion->invalid_value = (double)value.as_int8[0];
                        break;
                    case 3:
#ifndef WORDS_BIGENDIAN
                        swap_int16(value.as_int16);
#endif
                        conversion->invalid_value = (double)value.as_int16[0];
                        break;
                    case 4:
#ifndef WORDS_BIGENDIAN
                        swap_int32(value.as_int32);
#endif
                        conversion->invalid_value = (double)value.as_int32[0];
                        break;
                    case 5:
#ifndef WORDS_BIGENDIAN
                        swap_float(value.as_float);
#endif
                        conversion->invalid_value = (double)value.as_float[0];
                        break;
                    case 6:
#ifndef WORDS_BIGENDIAN
                        swap_double(value.as_double);
#endif
                        conversion->invalid_value = value.as_double[0];
                        break;
                    default:
                        assert(0);
                        exit(1);
                }
            }
        }

        if (nc_type == 2)       /* treat char arrays as strings */
        {
            basic_type = coda_netcdf_basic_type_new(nc_type, offset, 0, nelems);
        }
        else
        {
            basic_type = coda_netcdf_basic_type_new(nc_type, offset, 0, 1);
        }
        if (basic_type == NULL)
        {
            free(name);
            coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
            return -1;
        }

        if (nc_type == 2 || nelems == 1)
        {
            if (coda_mem_record_add_field(*attributes, name, (coda_dynamic_type *)basic_type, 1) != 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)basic_type);
                free(name);
                coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                return -1;
            }
        }
        else
        {
            coda_netcdf_array *array;
            long size = nelems;

            array = coda_netcdf_array_new(1, &size, basic_type);
            if (array == NULL)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)basic_type);
                free(name);
                coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                return -1;
            }
            if (coda_mem_record_add_field(*attributes, name, (coda_dynamic_type *)array, 1) != 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)array);
                free(name);
                coda_dynamic_type_delete((coda_dynamic_type *)*attributes);
                return -1;
            }
        }
        free(name);
    }

    return 0;
}

static int read_var_array(coda_netcdf_product *product, int32_t num_dim_lengths, int32_t *dim_length,
                          int appendable_dim, coda_mem_record *root)
{
    int32_t tag;
    int32_t num_var;
    long i;

    if (read(product->fd, &tag, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&tag);
#endif

    if (read(product->fd, &num_var, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename, strerror(errno));
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&num_var);
#endif

    if (tag == 0)
    {
        if (num_var != 0)
        {
            coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for nelems for empty var_array) "
                           "for file %s", product->filename);
            return -1;
        }
        return 0;
    }

    if (tag != 11)      /* NC_VARIABLE */
    {
        coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid value for NC_VARIABLE tag) for file %s",
                       product->filename);
        return -1;
    }

    for (i = 0; i < num_var; i++)
    {
        coda_netcdf_basic_type *basic_type;
        coda_mem_record *attributes = NULL;
        coda_conversion *conversion;
        long dim[CODA_MAX_NUM_DIMS];
        int32_t string_length;
        int64_t offset;
        int32_t nc_type;
        int32_t nelems;
        int32_t vsize;
        int32_t dim_id;
        char *name;
        long last_dim_length = 0;
        int last_dim_set = 0;
        int record_var = 0;
        int num_dims;
        long j;

        /* nelems */
        if (read(product->fd, &string_length, 4) < 0)
        {
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&string_length);
#endif
        /* chars */
        name = malloc(string_length + 1);
        if (name == NULL)
        {
            coda_set_error(CODA_ERROR_OUT_OF_MEMORY, "out of memory (could not allocate %lu bytes) (%s:%u)",
                           (long)(string_length + 1), __FILE__, __LINE__);
            return -1;
        }
        name[string_length] = '\0';
        if (read(product->fd, name, string_length) < 0)
        {
            free(name);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
        /* chars padding */
        if ((string_length & 3) != 0)
        {
            if (lseek(product->fd, 4 - (string_length & 3), SEEK_CUR) < 0)
            {
                free(name);
                coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                               strerror(errno));
                return -1;
            }
        }
        /* nelems */
        if (read(product->fd, &nelems, 4) < 0)
        {
            free(name);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&nelems);
#endif
        num_dims = 0;
        for (j = 0; j < nelems; j++)
        {
            /* dimid */
            if (read(product->fd, &dim_id, 4) < 0)
            {
                free(name);
                coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                               strerror(errno));
                return -1;
            }
#ifndef WORDS_BIGENDIAN
            swap_int32(&dim_id);
#endif
            if (dim_id < 0 || dim_id >= num_dim_lengths)
            {
                free(name);
                coda_set_error(CODA_ERROR_PRODUCT, "invalid netCDF file (invalid dimid for variable %s) for file "
                               "%s", name, product->filename);
                return -1;
            }
            if (j == nelems - 1)
            {
                last_dim_length = dim_length[dim_id];
                last_dim_set = 1;
            }
            else
            {
                if (j < CODA_MAX_NUM_DIMS)
                {
                    dim[j] = dim_length[dim_id];
                    num_dims++;
                }
                else
                {
                    dim[CODA_MAX_NUM_DIMS - 1] *= dim_length[dim_id];
                }
            }
            if (j == 0)
            {
                record_var = (dim_id == appendable_dim);
            }
        }

        conversion = coda_conversion_new(1.0, 1.0, 0.0, coda_NaN());
        if (conversion == NULL)
        {
            free(name);
            return -1;
        }
        /* vatt_array */
        if (read_att_array(product, &attributes, conversion) != 0)
        {
            coda_conversion_delete(conversion);
            free(name);
            return -1;
        }
        if (conversion->numerator == 1.0 && conversion->add_offset == 0.0 && coda_isNaN(conversion->invalid_value))
        {
            coda_conversion_delete(conversion);
            conversion = NULL;
        }

        /* nc_type */
        if (read(product->fd, &nc_type, 4) < 0)
        {
            coda_dynamic_type_delete((coda_dynamic_type *)attributes);
            coda_conversion_delete(conversion);
            free(name);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&nc_type);
#endif
        /* vsize */
        if (read(product->fd, &vsize, 4) < 0)
        {
            coda_dynamic_type_delete((coda_dynamic_type *)attributes);
            coda_conversion_delete(conversion);
            free(name);
            coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                           strerror(errno));
            return -1;
        }
#ifndef WORDS_BIGENDIAN
        swap_int32(&vsize);
#endif
        if (record_var)
        {
            product->record_size += vsize;
        }

        /* offset */
        if (product->netcdf_version == 1)
        {
            int32_t offset32;

            if (read(product->fd, &offset32, 4) < 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                coda_conversion_delete(conversion);
                free(name);
                coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                               strerror(errno));
                return -1;
            }
#ifndef WORDS_BIGENDIAN
            swap_int32(&offset32);
#endif
            offset = (int64_t)offset32;
        }
        else
        {
            if (read(product->fd, &offset, 8) < 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                coda_conversion_delete(conversion);
                free(name);
                coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product->filename,
                               strerror(errno));
                return -1;
            }
#ifndef WORDS_BIGENDIAN
            swap_int64(&offset);
#endif
        }
        if (last_dim_set)
        {
            if (nc_type == 2 && !(num_dims == 0 && record_var))
            {
                /* we treat the last dimension of a char array as a string */
                /* except if it is a one dimensional char array were the first dimension is the appendable dimension */
                basic_type = coda_netcdf_basic_type_new(nc_type, offset, record_var, last_dim_length);
            }
            else
            {
                basic_type = coda_netcdf_basic_type_new(nc_type, offset, record_var, 1);
                if (num_dims < CODA_MAX_NUM_DIMS)
                {
                    dim[num_dims] = last_dim_length;
                    num_dims++;
                }
                else
                {
                    dim[CODA_MAX_NUM_DIMS - 1] *= last_dim_length;
                }
            }
        }
        else
        {
            /* we only get here if the variable is a real scalar (num_dims == 0) */
            basic_type = coda_netcdf_basic_type_new(nc_type, offset, 0, 1);
        }
        if (basic_type == NULL)
        {
            coda_dynamic_type_delete((coda_dynamic_type *)attributes);
            coda_conversion_delete(conversion);
            free(name);
            return -1;
        }
        if (conversion != NULL)
        {
            if (coda_netcdf_basic_type_set_conversion(basic_type, conversion) != 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                coda_conversion_delete(conversion);
                free(name);
                return -1;
            }
        }
        if (num_dims == 0)
        {
            if (attributes != NULL)
            {
                if (coda_netcdf_basic_type_set_attributes(basic_type, attributes) != 0)
                {
                    coda_dynamic_type_delete((coda_dynamic_type *)basic_type);
                    coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                    free(name);
                    return -1;
                }
            }
            if (coda_mem_record_add_field(root, name, (coda_dynamic_type *)basic_type, 1) != 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)basic_type);
                free(name);
                return -1;
            }
        }
        else
        {
            coda_netcdf_array *array;

            array = coda_netcdf_array_new(num_dims, dim, basic_type);
            if (array == NULL)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)basic_type);
                coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                free(name);
                return -1;
            }
            if (attributes != NULL)
            {
                if (coda_netcdf_array_set_attributes(array, attributes) != 0)
                {
                    coda_dynamic_type_delete((coda_dynamic_type *)array);
                    coda_dynamic_type_delete((coda_dynamic_type *)attributes);
                    free(name);
                    return -1;
                }
            }
            if (coda_mem_record_add_field(root, name, (coda_dynamic_type *)array, 1) != 0)
            {
                coda_dynamic_type_delete((coda_dynamic_type *)array);
                free(name);
                return -1;
            }
        }
        free(name);
    }

    return 0;
}

int coda_netcdf_open(const char *filename, int64_t file_size, const coda_product_definition *definition,
                     coda_product **product)
{
    coda_netcdf_product *product_file;
    coda_type_record *root_definition;
    coda_mem_record *attributes;
    coda_mem_record *root;
    char magic[4];
    int open_flags;
    int32_t num_records;
    int32_t num_dims;
    int32_t *dim_length = NULL;
    int appendable_dim = -1;

    product_file = (coda_netcdf_product *)malloc(sizeof(coda_netcdf_product));
    if (product_file == NULL)
    {
        coda_set_error(CODA_ERROR_OUT_OF_MEMORY, "out of memory (could not allocate %lu bytes) (%s:%u)",
                       sizeof(coda_netcdf_product), __FILE__, __LINE__);
        return -1;
    }
    product_file->filename = NULL;
    product_file->file_size = file_size;
    product_file->format = coda_format_netcdf;
    product_file->root_type = NULL;
    product_file->product_definition = definition;
    product_file->product_variable_size = NULL;
    product_file->product_variable = NULL;
#if CODA_USE_QIAP
    product_file->qiap_info = NULL;
#endif
    product_file->use_mmap = 0;
    product_file->fd = -1;
    product_file->mmap_ptr = NULL;
#ifdef WIN32
    product_file->file_mapping = INVALID_HANDLE_VALUE;
    product_file->file = INVALID_HANDLE_VALUE;
#endif
    product_file->netcdf_version = 1;
    product_file->record_size = 0;

    product_file->filename = strdup(filename);
    if (product_file->filename == NULL)
    {
        coda_set_error(CODA_ERROR_OUT_OF_MEMORY, "out of memory (could not duplicate filename string) (%s:%u)",
                       __FILE__, __LINE__);
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }

    open_flags = O_RDONLY;
#ifdef WIN32
    open_flags |= _O_BINARY;
#endif
    product_file->fd = open(filename, open_flags);
    if (product_file->fd < 0)
    {
        coda_set_error(CODA_ERROR_FILE_OPEN, "could not open file %s (%s)", filename, strerror(errno));
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }

    /* create root type */
    root_definition = coda_type_record_new(coda_format_netcdf);
    if (root_definition == NULL)
    {
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }
    root = coda_mem_record_new(root_definition);
    coda_type_release((coda_type *)root_definition);
    if (root == NULL)
    {
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }
    product_file->root_type = (coda_dynamic_type *)root;

    /* magic */
    if (read(product_file->fd, magic, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product_file->filename,
                       strerror(errno));
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }
    assert(magic[0] == 'C' && magic[1] == 'D' && magic[2] == 'F');
    product_file->netcdf_version = magic[3];
    if (product_file->netcdf_version != 1 && product_file->netcdf_version != 2)
    {
        coda_set_error(CODA_ERROR_UNSUPPORTED_PRODUCT, "not a supported format version (%d) of the netCDF format for "
                       "file %s", product_file->netcdf_version, product_file->filename);
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }

    /* numrecs */
    if (read(product_file->fd, &num_records, 4) < 0)
    {
        coda_set_error(CODA_ERROR_FILE_READ, "could not read from file %s (%s)", product_file->filename,
                       strerror(errno));
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }
#ifndef WORDS_BIGENDIAN
    swap_int32(&num_records);
#endif

    /* dim_array */
    if (read_dim_array(product_file, num_records, &num_dims, &dim_length, &appendable_dim) != 0)
    {
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }

    /* gatt_array */
    if (read_att_array(product_file, &attributes, NULL) != 0)
    {
        if (dim_length != NULL)
        {
            free(dim_length);
        }
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }
    if (attributes != NULL)
    {
        if (coda_mem_type_set_attributes((coda_mem_type *)root, (coda_dynamic_type *)attributes, 1) != 0)
        {
            if (dim_length != NULL)
            {
                free(dim_length);
            }
            coda_netcdf_close((coda_product *)product_file);
            return -1;
        }
    }

    /* var_array */
    if (read_var_array(product_file, num_dims, dim_length, appendable_dim, root) != 0)
    {
        if (dim_length != NULL)
        {
            free(dim_length);
        }
        coda_netcdf_close((coda_product *)product_file);
        return -1;
    }

    if (dim_length != NULL)
    {
        free(dim_length);
    }

    if (coda_option_use_mmap)
    {
        /* Perform an mmap() of the file, filling the following fields:
         *   product->use_mmap = 1
         *   product->file         (windows only )
         *   product->file_mapping (windows only )
         *   product->mmap_ptr     (windows, *nix)
         */
        product_file->use_mmap = 1;
#ifdef WIN32
        close(product_file->fd);
        product_file->file = CreateFile(product_file->filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, NULL);
        if (product_file->file == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                coda_set_error(CODA_ERROR_FILE_NOT_FOUND, "could not find %s", product_file->filename);
            }
            else
            {
                LPVOID lpMsgBuf;

                if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                  FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL) == 0)
                {
                    /* Set error without additional information */
                    coda_set_error(CODA_ERROR_FILE_OPEN, "could not open file %s", product_file->filename);
                }
                else
                {
                    coda_set_error(CODA_ERROR_FILE_OPEN, "could not open file %s (%s)", product_file->filename,
                                   (LPCTSTR) lpMsgBuf);
                    LocalFree(lpMsgBuf);
                }
            }
            coda_netcdf_close((coda_product *)product_file);
            return -1;  /* indicate failure */
        }

        /* Try to do file mapping */
        product_file->file_mapping = CreateFileMapping(product_file->file, NULL, PAGE_READONLY, 0,
                                                       (int32_t)product_file->file_size, 0);
        if (product_file->file_mapping == NULL)
        {
            LPVOID lpMsgBuf;

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL) == 0)
            {
                /* Set error without additional information */
                coda_set_error(CODA_ERROR_FILE_OPEN, "could not map file %s into memory", product_file->filename);
            }
            else
            {
                coda_set_error(CODA_ERROR_FILE_OPEN, "could not map file %s into memory (%s)", product_file->filename,
                               (LPCTSTR) lpMsgBuf);
                LocalFree(lpMsgBuf);
            }
            coda_netcdf_close((coda_product *)product_file);
            return -1;
        }

        product_file->mmap_ptr = (uint8_t *)MapViewOfFile(product_file->file_mapping, FILE_MAP_READ, 0, 0, 0);
        if (product_file->mmap_ptr == NULL)
        {
            LPVOID lpMsgBuf;

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL) == 0)
            {
                /* Set error without additional information */
                coda_set_error(CODA_ERROR_FILE_OPEN, "could not map file %s into memory", product_file->filename);
            }
            else
            {
                coda_set_error(CODA_ERROR_FILE_OPEN, "could not map file %s into memory (%s)", product_file->filename,
                               (LPCTSTR) lpMsgBuf);
                LocalFree(lpMsgBuf);
            }
            coda_netcdf_close((coda_product *)product_file);
            return -1;
        }
#else
        product_file->mmap_ptr = (uint8_t *)mmap(0, product_file->file_size, PROT_READ, MAP_SHARED, product_file->fd,
                                                 0);
        if (product_file->mmap_ptr == (uint8_t *)MAP_FAILED)
        {
            coda_set_error(CODA_ERROR_FILE_OPEN, "could not map file %s into memory (%s)", product_file->filename,
                           strerror(errno));
            product_file->mmap_ptr = NULL;
            close(product_file->fd);
            coda_netcdf_close((coda_product *)product_file);
            return -1;
        }

        /* close file descriptor (the file handle is not needed anymore) */
        close(product_file->fd);
#endif
    }

    *product = (coda_product *)product_file;

    return 0;
}

int coda_netcdf_close(coda_product *product)
{
    coda_netcdf_product *product_file = (coda_netcdf_product *)product;

    if (product_file->use_mmap)
    {
#ifdef WIN32
        if (product_file->mmap_ptr != NULL)
        {
            UnmapViewOfFile(product_file->mmap_ptr);
        }
        if (product_file->file_mapping != INVALID_HANDLE_VALUE)
        {
            CloseHandle(product_file->file_mapping);
        }
        if (product_file->file != INVALID_HANDLE_VALUE)
        {
            CloseHandle(product_file->file);
        }
#else
        if (product_file->mmap_ptr != NULL)
        {
            munmap((void *)product_file->mmap_ptr, product_file->file_size);
        }
#endif
    }
    else
    {
        if (product_file->fd >= 0)
        {
            close(product_file->fd);
        }
    }

    if (product_file->root_type != NULL)
    {
        coda_dynamic_type_delete(product_file->root_type);
    }

    if (product_file->filename != NULL)
    {
        free(product_file->filename);
    }

    free(product_file);

    return 0;
}
