#include <stdio.h>
#include <atomic>
#include <map>
#include <string>
#include <vector>

#include "../../attachment.hpp"
#include "../../backend.hpp"
#include "../../internal.hpp"
#include "../../options.hpp"
#include "../../path.hpp"

#include "client/crash_report_database.h"
#include "client/crashpad_client.h"
#include "client/crashpad_info.h"
#include "client/settings.h"

using namespace crashpad;

static SimpleStringDictionary simple_annotations;
static base::FilePath s_database;

namespace sentry {

void init_backend() {
    const sentry_options_t *options = sentry_get_options();

    s_database = base::FilePath(options->database_path.as_osstr());
    base::FilePath handler(options->handler_path.as_osstr());
    std::map<std::string, std::string> annotations;
    std::map<std::string, base::FilePath> file_attachments;

    for (const sentry::Attachment &attachment : options->attachments) {
        file_attachments.emplace(attachment.name(),
                                 base::FilePath(attachment.path().as_osstr()));
    }

    std::vector<std::string> arguments;
    arguments.push_back("--no-rate-limit");

#ifdef _WIN32
    // Temporary fix for Windows
    arguments.push_back("--no-upload-gzip");
#endif

    CrashpadClient client;
    std::string url = options->dsn.get_minidump_url();
    bool success = client.StartHandlerWithAttachments(
        handler, s_database, s_database, url, annotations, file_attachments,
        arguments,
        /* restartable */ true,
        /* asynchronous_start */ false);

    if (success) {
        SENTRY_LOG("Started client handler.");
    } else {
        SENTRY_LOG("Failed to start client handler.");
        return;
    }

    std::unique_ptr<CrashReportDatabase> db = CrashReportDatabase::Initialize(s_database);

    if (db != nullptr && db->GetSettings() != nullptr) {
        db->GetSettings()->SetUploadsEnabled(true);
    }

    CrashpadInfo *crashpad_info = CrashpadInfo::GetCrashpadInfo();
    crashpad_info->set_simple_annotations(&simple_annotations);
}

void enable_backend(bool enabled){
    std::unique_ptr<CrashReportDatabase> db = CrashReportDatabase::InitializeWithoutCreating(s_database);
    if (db != nullptr && db->GetSettings() != nullptr) {
        db->GetSettings()->SetUploadsEnabled(enabled);
    }
}

void dump_without_crash() {
    CONTEXT ctx;
    CaptureContext(&ctx);
    CrashpadClient::DumpWithoutCrash(ctx);
}

void dump_and_crash(void* ctx) {
    CrashpadClient::DumpAndCrash(reinterpret_cast<EXCEPTION_POINTERS*>(ctx));
}

}  // namespace sentry
