#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>

#include <sqlite_orm/aasset_vfs.h>
#include <sqlite_orm/database.h>

using namespace sqlite;

extern "C" JNIEXPORT jstring JNICALL
Java_com_ondesly_sqliteormtest_ExampleInstrumentedTest_read(
        JNIEnv *env, jobject, jobject assetManager) {
    auto asset_manager = AAssetManager_fromJava(env, assetManager);
    register_aasset_vfs(asset_manager);

    sqlite3 *sqlite_db = nullptr;
    sqlite3_open_v2("test.db", &sqlite_db, SQLITE_OPEN_READONLY, default_vfs_name);

    //

    struct data {
        int id;
        int number;
        std::string text;
    };

    auto db = sqlite::database<data>::create(sqlite_db);
    db->set_fields({{&data::id,     "id"},
                    {&data::number, "number"},
                    {&data::text,   "text"}});

    //

    const auto table = "test_table";

    *db << SELECT << ALL << FROM << table << ';';

    std::vector<std::shared_ptr<data>> data;
    *db >> data;

    //

    std::string result;
    if (data.empty()) {
        result = "";
    } else {
        result = data.front()->text;
    }

    return env->NewStringUTF(result.c_str());
}