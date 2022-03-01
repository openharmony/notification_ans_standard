/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>

#include "gtest/gtest.h"

#include "distributed_database.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class DistributedDatabaseTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

public:
    virtual void OnInsert(const std::string &deviceId, const std::string &key, const std::string &value);
    virtual void OnUpdate(const std::string &deviceId, const std::string &key, const std::string &value);
    virtual void OnDelete(const std::string &deviceId, const std::string &key, const std::string &value);
    virtual void OnConnected(const std::string &deviceId);
    virtual void OnDisconnected(const std::string &deviceId);

protected:
    std::shared_ptr<DistributedDatabase> database_;
    std::shared_ptr<DistributedDatabaseCallback> databaseCallback_;
    std::shared_ptr<DistributedDeviceCallback> deviceCallback_;
};

void DistributedDatabaseTest::SetUp()
{
    DistributedDatabaseCallback::IDatabaseChange databaseCallback = {
        .OnInsert = std::bind(&DistributedDatabaseTest::OnInsert,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnUpdate = std::bind(&DistributedDatabaseTest::OnUpdate,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnDelete = std::bind(&DistributedDatabaseTest::OnDelete,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
    };
    DistributedDeviceCallback::IDeviceChange deviceCallback = {
        .OnConnected = std::bind(&DistributedDatabaseTest::OnConnected, this, std::placeholders::_1),
        .OnDisconnected = std::bind(&DistributedDatabaseTest::OnDisconnected, this, std::placeholders::_1),
    };

    databaseCallback_ = std::make_shared<DistributedDatabaseCallback>(databaseCallback);
    deviceCallback_ = std::make_shared<DistributedDeviceCallback>(deviceCallback);
    database_ = std::make_shared<DistributedDatabase>(databaseCallback_, deviceCallback_);
}

void DistributedDatabaseTest::TearDown()
{
    database_ = nullptr;
    databaseCallback_ = nullptr;
    deviceCallback_ = nullptr;
}

void DistributedDatabaseTest::OnInsert(const std::string &deviceId, const std::string &key, const std::string &value)
{}

void DistributedDatabaseTest::OnUpdate(const std::string &deviceId, const std::string &key, const std::string &value)
{}

void DistributedDatabaseTest::OnDelete(const std::string &deviceId, const std::string &key, const std::string &value)
{}

void DistributedDatabaseTest::OnConnected(const std::string &deviceId)
{}

void DistributedDatabaseTest::OnDisconnected(const std::string &deviceId)
{}

/**
 * @tc.name      : DistributedDatabase_PutToDistributedDB_00100
 * @tc.number    : PutToDistributedDB_00100
 * @tc.desc      : Put a key-value.
 */
HWTEST_F(DistributedDatabaseTest, PutToDistributedDB_00100, Function | SmallTest | Level1)
{
    std::string key("<key>");
    std::string value("<value>");

    EXPECT_EQ(database_->PutToDistributedDB(key, value), true);
}

/**
 * @tc.name      : DistributedDatabase_GetFromDistributedDB_00100
 * @tc.number    : GetFromDistributedDB_00100
 * @tc.desc      : Get value by its key.
 */
HWTEST_F(DistributedDatabaseTest, GetFromDistributedDB_00100, Function | SmallTest | Level1)
{
    std::string key("<key>");
    std::string value;

    EXPECT_EQ(database_->GetFromDistributedDB(key, value), true);
}

/**
 * @tc.name      : DistributedDatabase_GetFromDistributedDB_00200
 * @tc.number    : GetFromDistributedDB_00200
 * @tc.desc      : Get all entries which key start with prefixKey.
 */
HWTEST_F(DistributedDatabaseTest, GetFromDistributedDB_00200, Function | SmallTest | Level1)
{
    std::string prifixKey("<");
    std::vector<DistributedDatabase::Entry> entries;

    EXPECT_EQ(database_->GetEntriesFromDistributedDB(prifixKey, entries), true);
}

/**
 * @tc.name      : DistributedDatabase_DeleteToDistributedDB_00100
 * @tc.number    : DeleteToDistributedDB_00100
 * @tc.desc      : Delete a key-value with its key.
 */
HWTEST_F(DistributedDatabaseTest, DeleteToDistributedDB_00100, Function | SmallTest | Level1)
{
    std::string key("<key>");

    EXPECT_EQ(database_->DeleteToDistributedDB(key), true);
}

/**
 * @tc.name      : DistributedDatabase_ClearDataByDevice_00100
 * @tc.number    : ClearDataByDevice_00100
 * @tc.desc      : Remove the device data from remote.
 */
HWTEST_F(DistributedDatabaseTest, ClearDataByDevice_00100, Function | SmallTest | Level1)
{
    std::string deviceId("<remoteDeviceId>");

    EXPECT_EQ(database_->ClearDataByDevice(deviceId), true);
}

/**
 * @tc.name      : DistributedDatabase_GetLocalDeviceId_00100
 * @tc.number    : GetLocalDeviceId_00100
 * @tc.desc      : Get local device id.
 */
HWTEST_F(DistributedDatabaseTest, GetLocalDeviceId_00100, Function | SmallTest | Level1)
{
    std::string deviceId;

    EXPECT_EQ(database_->GetLocalDeviceId(deviceId), true);
}

/**
 * @tc.name      : DistributedDatabase_GetLocalDeviceInfo_00100
 * @tc.number    : GetLocalDeviceInfo_00100
 * @tc.desc      : Get local device information.
 */
HWTEST_F(DistributedDatabaseTest, GetLocalDeviceInfo_00100, Function | SmallTest | Level1)
{
    DistributedDatabase::DeviceInfo deviceInfo;

    EXPECT_EQ(database_->GetLocalDeviceInfo(deviceInfo), true);
}

/**
 * @tc.name      : DistributedDatabase_GetDeviceInfoList_00100
 * @tc.number    : GetDeviceInfoList_00100
 * @tc.desc      : Get infomations for all devices.
 */
HWTEST_F(DistributedDatabaseTest, GetDeviceInfoList_00100, Function | SmallTest | Level1)
{
    std::vector<DistributedDatabase::DeviceInfo> deviceInfos;

    EXPECT_EQ(database_->GetDeviceInfoList(deviceInfos), true);
}
}  // namespace Notification
}  // namespace OHOS