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

#include <gtest/gtest.h>

#include "celix/Properties.h"

using ::testing::MatchesRegex;

class CxxPropertiesTestSuite : public ::testing::Test {
public:
};

TEST_F(CxxPropertiesTestSuite, testCreateDestroy) {
    celix::Properties props{};
    EXPECT_EQ(0, props.size());
}

TEST_F(CxxPropertiesTestSuite, testFillAndLoop) {
    celix::Properties props{};
    EXPECT_EQ(0, props.size());

    props["key1"] = "value1";
    props.set("key2", "value2");
    props.set("key3", 3.3);
    props.set("key4", 4);
    props.set("key5", true);
    EXPECT_EQ(5, props.size());

    EXPECT_EQ(props.get("key1"), "value1");
    EXPECT_EQ(props.get("key2"), "value2");
    EXPECT_EQ(props.getAsDouble("key3", 0), 3.3);
    EXPECT_EQ(props.get("key4"), "4");
    EXPECT_EQ(props.getAsLong("key4", -1), 4);
    EXPECT_EQ(props.get("key5"), "true");
    EXPECT_EQ(props.getAsBool("key5", false), true);

    int count = 0;
    for (auto it = props.begin(); it != props.end(); ++it) {
        EXPECT_NE(it.first, "");
        count++;
    }
    EXPECT_EQ(5, count);

    count = 0;
    for (const auto& pair : props) {
        EXPECT_NE(pair.first, "");
        count++;
    }
    EXPECT_EQ(5, count);
}

TEST_F(CxxPropertiesTestSuite, testLoopForSize0And1) {
    celix::Properties props0{};
    for (const auto& pair : props0) {
        FAIL() << "Should not get an loop entry with a properties size of 0. got key: " << pair.first;
    }

    celix::Properties props1{};
    props1.set("key1", "value1");
    int count = 0;
    for (const auto& pair : props1) {
        EXPECT_EQ(pair.first, "key1");
        count++;
    }
    EXPECT_EQ(1, count);
}

TEST_F(CxxPropertiesTestSuite, testCopy) {
    celix::Properties props{};

    props["key1"] = "value1";
    props["key2"] = "value2";

    celix::Properties copy = props;
    copy["key1"] = "value1_new";

    std::string v1 = props["key1"];
    std::string v2 =  copy["key1"];
    EXPECT_EQ(v1, "value1");
    EXPECT_EQ(v2, "value1_new");
}

TEST_F(CxxPropertiesTestSuite, testWrap) {
    auto *props = celix_properties_create();
    celix_properties_set(props, "test", "test");

    EXPECT_EQ(1, celix_properties_size(props));
    {
        auto cxxProps = celix::Properties::wrap(props);
        EXPECT_EQ(1, cxxProps->size());
        EXPECT_EQ(props, cxxProps->getCProperties());
    } //NOTE cxxProps out of scope, but will not destroy celix_properties
    EXPECT_EQ(1, celix_properties_size(props));

    celix_properties_destroy(props);
}

TEST_F(CxxPropertiesTestSuite, getType) {
    celix::Properties props{};

    props.set("bool", true);
    props.set("long1", 1l);
    props.set("long2", (int)1); //should lead to long;
    props.set("long3", (unsigned int)1); //should lead to long;
    props.set("long4", (short)1); //should lead to long;
    props.set("long5", (unsigned short)1); //should lead to long;
    props.set("long6", (char)1); //should lead to long;
    props.set("long7", (unsigned char)1); //should lead to long;
    props.set("double1", 1.0);
    props.set("double2", 1.0f); //set float should lead to double
    props.set("version", celix::Version{1, 2, 3});

    EXPECT_EQ(props.getType("bool"), celix::Properties::ValueType::Bool);
    EXPECT_EQ(props.getType("long1"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long2"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long3"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long4"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long5"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long6"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("long7"), celix::Properties::ValueType::Long);
    EXPECT_EQ(props.getType("double1"), celix::Properties::ValueType::Double);
    EXPECT_EQ(props.getType("double2"), celix::Properties::ValueType::Double);
    EXPECT_EQ(props.getType("version"), celix::Properties::ValueType::Version);
}

TEST_F(CxxPropertiesTestSuite, testGetAsVersion) {
    celix::Properties props;

    // Test getting a version from a string property
    props.set("key", "1.2.3");
    celix::Version ver{1, 2, 3};
    EXPECT_TRUE(props.getAsVersion("key") == ver);

    // Test getting a version from a version property
    props.set("key", celix::Version{2, 3, 4});
    ver = celix::Version{2, 3, 4};
    EXPECT_EQ(props.getAsVersion("key"), ver);

    // Test getting default value when property is not set
    ver = celix::Version{3, 4, 5};
    EXPECT_EQ(props.getAsVersion("non_existent_key", celix::Version{3, 4, 5}), ver);

    // Test getting default value when property value is not a valid version string
    props.set("key", "invalid_version_string");
    ver = celix::Version{4, 5, 6};
    EXPECT_EQ(props.getAsVersion("key", celix::Version{4, 5, 6}), ver);
}

#if __cplusplus >= 201703L //C++17 or higher
TEST_F(CxxPropertiesTestSuite, testStringView) {
    constexpr std::string_view stringViewKey = "KEY1";
    constexpr std::string_view stringViewValue = "VALUE1";
    std::string stringKey{"KEY2"};
    std::string stringValue{"VALUE2"};
    const char* charKey = "KEY3";
    const char* charValue = "VALUE3";

    {
        //rule: I can create properties with initializer_list using std::string, const char* and string_view
        celix::Properties props {
                {charKey, charValue},
                {stringKey, stringValue},
                {stringViewKey, stringViewValue}
        };
        EXPECT_EQ(props.size(), 3);

        //rule: I can use the subscript operator using string_view objects
        constexpr std::string_view k = "KEY2";
        constexpr std::string_view v = "VALUE_NEW";
        std::string check = props[k];
        EXPECT_EQ(check, "VALUE2");
        props[k] = v;
        check = props[k];
        EXPECT_EQ(check, v);
    }

    {
        //rule: I can use set/get with string_view
        constexpr std::string_view key = "TEST_KEY";
        constexpr std::string_view value = "TEST_VALUE";

        celix::Properties props{};

        props.set(key, value);
        EXPECT_EQ(value, props.get(key));
        props[key] = value;
        EXPECT_EQ(value, props.get(key));
        EXPECT_EQ(1, props.size());

        props.set(key,  std::string{"string value"});
        EXPECT_EQ("string value", props.get(key));
        props.set(key,  "string value");
        EXPECT_EQ("string value", props.get(key));
        EXPECT_EQ(1, props.size());

        props.set(key, 1L); //long
        EXPECT_EQ(1L, props.getAsLong(key, -1));
        props.set(key, 2.0); //double
        EXPECT_EQ(2.0, props.getAsDouble(key, -1));
        props.set(key, false); //bool
        EXPECT_EQ(false, props.getAsBool(key, true));
    }
}

TEST_F(CxxPropertiesTestSuite, testUseOfConstexprInSetMethod) {
    celix::Properties props{};

    //Test if different bool "types" are correctly handled
    props.set("key1", true);
    props.set<const bool>("key2", false);
    props.set<const bool&>("key3", 1);
    props.set<bool&&>("key4", 0);
    props.set<volatile bool&&>("key5", true);
    EXPECT_EQ(5, props.size());

    EXPECT_EQ(props.getAsBool("key1", false), true);
    EXPECT_EQ(props.get("key2"), "false");
    EXPECT_EQ(props.get("key3"), "true");
    EXPECT_EQ(props.get("key4"), "false");
    EXPECT_EQ(props.get("key5"), "true");
}
#endif
