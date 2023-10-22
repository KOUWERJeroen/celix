/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/**
 * @file celix_properties.h
 * @brief Header file for the Celix Properties API.
 *
 * The Celix Properties API provides a means for storing and manipulating key-value pairs, called properties,
 * which can be used to store configuration data or metadata for a service, component, or framework configuration.
 * Functions are provided for creating and destroying property sets, loading and storing properties from/to a file
 * or stream, and setting, getting, and unsetting individual properties. There are also functions for converting
 * property values to various types (e.g. long, bool, double) and for iterating over the properties in a set.
 *
 * Supported property value types include:
 *  - string (char*)
 *  - long
 *  - double
 *  - bool
 *  - celix_version_t*
 */

#include <stdio.h>

#include "celix_cleanup.h"
#include "celix_compiler.h"
#include "celix_errno.h"
#include "celix_utils_export.h"
#include "celix_version.h"

#ifndef CELIX_PROPERTIES_H_
#define CELIX_PROPERTIES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief celix_properties_t is a type that represents a set of key-value pairs called properties.
 *
 * @note Not thread safe.
 */
typedef struct celix_properties celix_properties_t;

/**
 * @brief Enum representing the possible types of a property value.
 */
typedef enum celix_properties_value_type {
    CELIX_PROPERTIES_VALUE_TYPE_UNSET = 0,  /**< Property value is not set. */
    CELIX_PROPERTIES_VALUE_TYPE_STRING = 1, /**< Property value is a string. */
    CELIX_PROPERTIES_VALUE_TYPE_LONG = 2,   /**< Property value is a long integer. */
    CELIX_PROPERTIES_VALUE_TYPE_DOUBLE = 3, /**< Property value is a double. */
    CELIX_PROPERTIES_VALUE_TYPE_BOOL = 4,   /**< Property value is a boolean. */
    CELIX_PROPERTIES_VALUE_TYPE_VERSION = 5 /**< Property value is a Celix version. */
} celix_properties_value_type_e;

/**
 * @brief A structure representing a single value entry in a property set.
 */
typedef struct celix_properties_entry {
    const char* value;                       /**< The string value or string representation of a non-string
                                                  typed value.*/
    celix_properties_value_type_e valueType; /**< The type of the value of the entry */

    union {
        const char* strValue;                /**< The string value of the entry. */
        long longValue;                      /**< The long integer value of the entry. */
        double doubleValue;                  /**< The double-precision floating point value of the entry. */
        bool boolValue;                      /**< The boolean value of the entry. */
        const celix_version_t* versionValue; /**< The Celix version value of the entry. */
    } typed;                                 /**< The typed values of the entry. Only valid if valueType
                                                  is not CELIX_PROPERTIES_VALUE_TYPE_UNSET and only the matching
                                                  value types should be used. E.g typed.boolValue if valueType is
                                                  CELIX_PROPERTIES_VALUE_TYPE_BOOL. */
} celix_properties_entry_t;

/**
 * @brief Represents an iterator for iterating over the entries in a celix_properties_t object.
 */
typedef struct celix_properties_iterator {
    /**
     * @brief The index of the current iterator.
     */
    size_t index;

    /**
     * @brief Te current key.
     */
    const char* key;

    /**
     * @brief The current value entry.
     */
    celix_properties_entry_t entry;

    /**
     * @brief Private data used to implement the iterator.
     */
    char _data[56];
} celix_properties_iterator_t;

/**
 * @brief Create a new empty property set.
 *
 * @return A new empty property set.
 */
CELIX_UTILS_EXPORT celix_properties_t* celix_properties_create();

/**
 * @brief Destroy a property set, freeing all associated resources.
 *
 * @param[in] properties The property set to destroy. If properties is NULL, this function will do nothing.
 */
CELIX_UTILS_EXPORT void celix_properties_destroy(celix_properties_t* properties);

CELIX_DEFINE_AUTOPTR_CLEANUP_FUNC(celix_properties_t, celix_properties_destroy)

/**
 * @brief Load properties from a file.
 *
 * @param[in] filename The name of the file to load properties from.
 * @return A property set containing the properties from the file.
 * @retval NULL If an error occurred (e.g. file not found).
 */
CELIX_UTILS_EXPORT celix_properties_t* celix_properties_load(const char* filename);

/**
 * @brief Load properties from a stream.
 *
 * @param[in,out] stream The stream to load properties from.
 * @return A property set containing the properties from the stream.
 * @retval NULL If an error occurred (e.g. invalid format).
 */
CELIX_UTILS_EXPORT celix_properties_t* celix_properties_loadWithStream(FILE* stream);

/**
 * @brief Load properties from a string.
 *
 * @param[in] input The string to load properties from.
 * @return A property set containing the properties from the string.
 * @retval NULL If an error occurred (e.g. invalid format).
 */
CELIX_UTILS_EXPORT celix_properties_t* celix_properties_loadFromString(const char* input);

/**
 * @brief Store properties to a file.
 *
 * @note Properties values are always stored as string values, regardless of their actual underlining types.
 *
 * @param[in] properties The property set to store.
 * @param[in] file The name of the file to store the properties to.
 * @param[in] header An optional - single line - header to write to the file before the properties.
 *                   Will be prefix with a '#' character.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_FILE_IO_EXCEPTION if there was an error writing to the
 *         file.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_store(celix_properties_t* properties,
                                                         const char* file,
                                                         const char* header);

/**
 * @brief Get the entry for a given key in a property set.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key to search for.
 * @return The entry for the given key, or a NULL if the key is not found.
 */
CELIX_UTILS_EXPORT celix_properties_entry_t* celix_properties_getEntry(const celix_properties_t* properties,
                                                                       const char* key);

/**
 * @brief Get the value of a property.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set.
 * @return The value of the property, or the default value if the property is not set.
 */
CELIX_UTILS_EXPORT const char*
celix_properties_get(const celix_properties_t* properties, const char* key, const char* defaultValue);

/**
 * @brief Get the type of a property value.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get the type of.
 * @return The type of the property value, or CELIX_PROPERTIES_VALUE_TYPE_UNSET if the property is not set.
 */
CELIX_UTILS_EXPORT celix_properties_value_type_e celix_properties_getType(const celix_properties_t* properties,
                                                                          const char* key);

/**
 * @brief Set the value of a property.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] value The value to set the property to.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_set(celix_properties_t* properties,
                                                       const char* key,
                                                       const char* value);

/**
 * @brief Set the value of a property without copying the value string.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set. This string will be used directly, so it must not be freed or
*                 modified after calling this function.
 * @param[in] value The value to set the property to. This string will be used directly, so it must not be freed or
 *                  modified after calling this function.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 *         When CELIX_ENOMEM is returned, the key and value strings are not used and freed by this function.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setWithoutCopy(celix_properties_t* properties,
                                                                  char* key,
                                                                  char* value);

/**
 * @brief Set the value of a property based on the provided property entry, maintaining underlying type.
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] entry The entry to set the property to. The entry will be copied, so it can be freed after calling
 *                  this function.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setEntry(celix_properties_t* properties,
                                                            const char* key,
                                                            const celix_properties_entry_t* entry);

/**
 * @brief Unset a property, removing it from the property set.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to unset.
 */
CELIX_UTILS_EXPORT void celix_properties_unset(celix_properties_t* properties, const char* key);

/**
 * @brief Make a copy of a property set.
 *
 * @param[in] properties The property set to copy.
 * @return A copy of the given property set.
 */
CELIX_UTILS_EXPORT celix_properties_t* celix_properties_copy(const celix_properties_t* properties);

/**
 * @brief Get the value of a property as a long integer.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set, the value is not a long integer,
 *                     or if the value cannot be converted to a long integer.
 * @return The value of the property as a long integer, or the default value if the property is not set,
 *         the value is not a long integer, or if the value cannot be converted to a long integer.
 *         If the value is a string, it will be converted to a long integer if possible.
 */
CELIX_UTILS_EXPORT long
celix_properties_getAsLong(const celix_properties_t* properties, const char* key, long defaultValue);

/**
 * @brief Set the value of a property to a long integer.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] value The long value to set the property to.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setLong(celix_properties_t* properties, const char* key, long value);

/**
 * @brief Get the value of a property as a boolean.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set, the value is not a boolean, or if the value
 *                     cannot be converted to a boolean.
 * @return The value of the property as a boolean, or the default value if the property is not set, the value is not a
 *         boolean, or if the value cannot be converted to a boolean. If the value is a string, it will be converted
 *         to a boolean if possible.
 */
CELIX_UTILS_EXPORT bool
celix_properties_getAsBool(const celix_properties_t* properties, const char* key, bool defaultValue);

/**
 * @brief Set the value of a property to a boolean.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] val The boolean value to set the property to.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setBool(celix_properties_t* properties, const char* key, bool val);

/**
 * @brief Set the value of a property to a double.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] val The double value to set the property to.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setDouble(celix_properties_t* properties,
                                                             const char* key,
                                                             double val);

/**
 * @brief Get the value of a property as a double.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set, the value is not a double,
 *                     or if the value cannot be converted to a double.
 * @return The value of the property as a double, or the default value if the property is not set, the value is not
 *         a double, or if the value cannot be converted to a double. If the value is a string, it will be converted
 *         to a double if possible.
 */
CELIX_UTILS_EXPORT double
celix_properties_getAsDouble(const celix_properties_t* properties, const char* key, double defaultValue);

/**
 * @brief Set the value of a property as a Celix version.
 *
 * This function will make a copy of the provided celix_version_t object and store it in the property set.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] version The value to set. The function will make a copy of this object and store it in the property set.
 * @return CELIX_SUCCESS if the operation was successful, CELIX_ENOMEM if there was not enough memory to set the entry.
 */
CELIX_UTILS_EXPORT celix_status_t celix_properties_setVersion(celix_properties_t* properties,
                                                              const char* key,
                                                              const celix_version_t* version);

/**
 * @brief Set the value of a property as a Celix version.
 *
 * This function will store a reference to the provided celix_version_t object in the property set and takes
 * ownership of the provided version.
 *
 * @param[in] properties The property set to modify.
 * @param[in] key The key of the property to set.
 * @param[in] version The value to set. The function will store a reference to this object in the property set and
 *                    takes ownership of the provided version.
 */
CELIX_UTILS_EXPORT void
celix_properties_setVersionWithoutCopy(celix_properties_t* properties, const char* key, celix_version_t* version);

/**
 * @brief Get the Celix version value of a property.
 *
 * This function does not convert a string property value to a Celix version automatically.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set or if the value is not a Celix version.
 * @return The value of the property if it is a Celix version, or the default value if the property is not set or the
 *         value is not a Celix version.
 */
CELIX_UTILS_EXPORT const celix_version_t*
celix_properties_getVersion(const celix_properties_t* properties, const char* key, const celix_version_t* defaultValue);

/**
 * @brief Get the value of a property as a Celix version.
 *
 * If the property value is a Celix version, a copy of this version will be returned.
 * If the property value is a string, this function will attempt to convert it to a new Celix version.
 * If the property is not set or is not a valid Celix version string, a copy of the provided defaultValue is returned.
 *
 * @note The caller is responsible for deallocating the memory of the returned version.
 *
 * @param[in] properties The property set to search.
 * @param[in] key The key of the property to get.
 * @param[in] defaultValue The value to return if the property is not set or if the value is not a Celix version.
 * @return A copy of the property value if it is a Celix version, or a new Celix version created from the property
 *         value string if it can be converted, or a copy of the default value if the property is not set or the value
 *         is not a valid Celix version.
 * @retval NULL if version cannot be found/converted and the defaultValue is NULL.
 */
CELIX_UTILS_EXPORT celix_version_t* celix_properties_getAsVersion(const celix_properties_t* properties,
                                                                  const char* key,
                                                                  const celix_version_t* defaultValue);

/**
 * @brief Get the number of properties in a property set.
 *
 * @param[in] properties The property set to get the size of.
 * @return The number of properties in the property set.
 */
CELIX_UTILS_EXPORT int celix_properties_size(const celix_properties_t* properties);

/**
 * @brief Construct an iterator pointing to the first entry in the properties object.
 *
 * @param[in] properties The properties object to iterate over.
 * @return The iterator pointing to the first entry in the properties object.
 */
CELIX_UTILS_EXPORT celix_properties_iterator_t celix_properties_begin(const celix_properties_t* properties);

/**
 * @brief Construct an iterator pointing to the past-the-end entry in the properties object.
 *
 * This iterator is used to mark the end of the properties object and is not associated with any element in the
 * properties object.
 *
 * @param[in] properties The properties object to iterate over.
 * @return The iterator pointing to the past-the-end entry in the properties object.
 */
CELIX_UTILS_EXPORT celix_properties_iterator_t celix_properties_end(const celix_properties_t* properties);

/**
 * @brief Advance the iterator to the next entry.
 *
 * @param[in, out] iter The iterator.
 */
CELIX_UTILS_EXPORT void celix_propertiesIterator_next(celix_properties_iterator_t* iter);

/**
 * @brief Determine whether the iterator is pointing to an end position.
 *
 * An iterator is at an end position if it has no more entries to visit.
 *
 * @param[in] iter The iterator.
 * @return true if the iterator is at an end position, false otherwise.
 */
CELIX_UTILS_EXPORT bool celix_propertiesIterator_isEnd(const celix_properties_iterator_t* iter);

/**
 * @brief Determine whether two iterators are equal.
 *
 * @param[in] a The first iterator to compare.
 * @param[in] b The second iterator to compare.
 * @return true if the iterators are equal, false otherwise.
 */
CELIX_UTILS_EXPORT bool celix_propertiesIterator_equals(const celix_properties_iterator_t* a,
                                                        const celix_properties_iterator_t* b);

/**
 * @brief Iterate over the entries in the specified celix_properties_t object.
 *
 * This macro allows you to easily iterate over the entries in a celix_properties_t object.
 * The loop variable `iterName` will be of type celix_properties_iterator_t and will contain the current
 * entry during each iteration.
 *
 * @param[in] props The properties object to iterate over.
 * @param[in] iterName The name of the iterator variable to use in the loop.
 *
 * Example usage:
 * @code{.c}
 * // Iterate over all entries in the properties object
 * CELIX_PROPERTIES_ITERATE(properties, iter) {
 *     // Print the key and value of the current entry
 *     printf("%s: %s\n", iter.entry.key, iter.entry.value);
 * }
 * @endcode
 */
#define CELIX_PROPERTIES_ITERATE(props, iterName)                                                                      \
    for (celix_properties_iterator_t iterName = celix_properties_begin((props));                                       \
         !celix_propertiesIterator_isEnd(&(iterName));                                                                 \
         celix_propertiesIterator_next(&(iterName)))

#ifdef __cplusplus
}
#endif

#endif /* CELIX_PROPERTIES_H_ */
