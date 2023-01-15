#include "env.h"

#include <util/folder/dirut.h>
#include <util/stream/file.h>
#include <util/system/env.h>
#include <util/folder/path.h>
#include <util/generic/singleton.h>

#include <library/cpp/json/json_reader.h>
#include <library/cpp/json/json_value.h>

TString ArcadiaFromCurrentLocation(TStringBuf where, TStringBuf path) {
    return (TFsPath(ArcadiaSourceRoot()) / TFsPath(where).Parent() / path).Fix();
}

TString BinaryPath(TStringBuf path) {
    return (TFsPath(BuildRoot()) / path).Fix();
}

TString GetArcadiaTestsData() {
    TString atdRoot = NPrivate::GetTestEnv().ArcadiaTestsDataDir;
    if (atdRoot) {
        return atdRoot;
    }

    TString path = NPrivate::GetCwd();
    const char pathsep = GetDirectorySeparator();
    while (!path.empty()) {
        TString dataDir = path + "/arcadia_tests_data";
        if (IsDir(dataDir)) {
            return dataDir;
        }

        size_t pos = path.find_last_of(pathsep);
        if (pos == TString::npos) {
            pos = 0;
        }
        path.erase(pos);
    }

    return {};
}

TString GetWorkPath() {
    TString workPath = NPrivate::GetTestEnv().WorkPath;
    if (workPath) {
        return workPath;
    }

    return NPrivate::GetCwd();
}

TFsPath GetOutputPath() {
    return GetWorkPath() + "/testing_out_stuff";
}

TString GetRamDrivePath() {
    return NPrivate::GetTestEnv().RamDrivePath;
}

TString GetOutputRamDrivePath() {
    return NPrivate::GetTestEnv().TestOutputRamDrivePath;
}

bool FromYaTest() {
    return NPrivate::GetTestEnv().IsRunningFromTest;
}

namespace NPrivate {
    TTestEnv::TTestEnv() {
        ReInitialize();
    }

    void TTestEnv::ReInitialize() {
        IsRunningFromTest = false;
        ArcadiaTestsDataDir = "";
        SourceRoot = "";
        BuildRoot = "";
        WorkPath = "";
        RamDrivePath = "";
        TestOutputRamDrivePath = "";

        const TString contextFilename = GetEnv("YA_TEST_CONTEXT_FILE");
        if (contextFilename) {
            NJson::TJsonValue context;
            NJson::ReadJsonTree(TFileInput(contextFilename).ReadAll(), &context);

            NJson::TJsonValue* value;

            value = context.GetValueByPath("runtime.source_root");
            if (value) {
                SourceRoot = value->GetStringSafe("");
            }

            value = context.GetValueByPath("runtime.build_root");
            if (value) {
                BuildRoot = value->GetStringSafe("");
            }

            value = context.GetValueByPath("runtime.atd_root");
            if (value) {
                ArcadiaTestsDataDir = value->GetStringSafe("");
            }

            value = context.GetValueByPath("runtime.work_path");
            if (value) {
                WorkPath = value->GetStringSafe("");
            }

            value = context.GetValueByPath("runtime.ram_drive_path");
            if (value) {
                RamDrivePath = value->GetStringSafe("");
            }

            value = context.GetValueByPath("runtime.test_output_ram_drive_path");
            if (value) {
                TestOutputRamDrivePath = value->GetStringSafe("");
            }
        }

        if (!SourceRoot) {
            SourceRoot = GetEnv("ARCADIA_SOURCE_ROOT");
        }

        if (!BuildRoot) {
            BuildRoot = GetEnv("ARCADIA_BUILD_ROOT");
        }

        if (!ArcadiaTestsDataDir) {
            ArcadiaTestsDataDir = GetEnv("ARCADIA_TESTS_DATA_DIR");
        }

        if (!WorkPath) {
            WorkPath = GetEnv("TEST_WORK_PATH");
        }

        if (!RamDrivePath) {
            RamDrivePath = GetEnv("YA_TEST_RAM_DRIVE_PATH");
        }

        if (!TestOutputRamDrivePath) {
            TestOutputRamDrivePath = GetEnv("YA_TEST_OUTPUT_RAM_DRIVE_PATH");
        }

        const TString fromEnv = GetEnv("YA_TEST_RUNNER");
        IsRunningFromTest = (fromEnv == "1");
    }

    TString GetCwd() {
        try {
            return NFs::CurrentWorkingDirectory();
        } catch (...) {
            return {};
        }
    }

    const TTestEnv& GetTestEnv() {
        return *Singleton<TTestEnv>();
    }
}
