#include "profile.h"
#define H64 "1111111111111111111111111111111111111111111111111111111111111111"
#define H40 "1111111111111111111111111111111111111111"
#define IMAGE "{\"repository\":\"fixture\",\"commit\":\"" H40 "\",\"blob\":\"" H40 "\",\"rawSha256\":\"" H64 "\",\"imageSha256\":\"" H64 "\"}"
#if defined(_WIN32)
#define PLATFORM "win32"
#define OWNER "S-1-5-21-1"
#define ENDPOINT "{\"kind\":\"named-pipe\",\"name\":\"\\\\.\\pipe\\fixture-service\"}"
#define LAUNCH_ENDPOINT "{\"kind\":\"named-pipe\",\"name\":\"\\\\.\\pipe\\fixture-launch\"}"
#else
#define PLATFORM "linux"
#define OWNER "1000"
#define ENDPOINT "{\"kind\":\"unix-socket\",\"name\":\"/fixture/service\"}"
#define LAUNCH_ENDPOINT "{\"kind\":\"unix-socket\",\"name\":\"/fixture/launch\"}"
#endif
/* Non-authoritative semantic fixtures, with intentionally fictional associations.
 * Positive parsing must never enroll these strings as original image/source pins. */
int slcli_profile_source_vectors(void) {
  static const unsigned char body[]=
    "{\"schema\":\"service-lasso.cli-host-profile.v3\",\"platform\":\"" PLATFORM "\","
    "\"serviceSource\":{\"repository\":\"fixture\",\"commit\":\"" H40 "\",\"imageSha256\":\"" H64 "\",\"sourceSha256\":\"" H64 "\"},"
    "\"endpoint\":" ENDPOINT ",\"owner\":{\"nativeId\":\"" OWNER "\"},"
    "\"store\":{\"provider\":\"fixture\",\"rootIdentity\":\"" H64 "\",\"keyVersion\":\"fixture\",\"maximumBytes\":4194304},"
    "\"clientImages\":{\"primarySha256\":\"" H64 "\",\"observerSha256\":\"" H64 "\",\"launcherSha256\":\"" H64 "\",\"libraryFacadeSourceSha256\":\"" H64 "\"},"
    "\"control\":{\"maximumBindings\":16,\"maximumReferencesPerBinding\":128,\"maximumServiceBytes\":1048576},"
    "\"retention\":{\"provider\":\"fixture\",\"rootIdentity\":\"" H64 "\",\"maximumBytes\":9007199254740991,\"maximumRecordBytes\":768000,\"minimumDays\":90},"
    "\"launch\":{\"endpoint\":" LAUNCH_ENDPOINT ",\"primaryImageBinding\":" IMAGE ",\"observerImageBinding\":" IMAGE ",\"launcherImageBinding\":" IMAGE ","
    "\"libraryTransportBinding\":{\"repository\":\"fixture\",\"commit\":\"" H40 "\",\"blob\":\"" H40 "\",\"rawSha256\":\"" H64 "\",\"nodeImageSha256\":\"" H64 "\",\"packageSourceSha256\":\"" H64 "\",\"inheritedChannelKind\":\"fixture\"},"
    "\"maximumRequestBytes\":16384,\"maximumResultBytes\":16384,\"maximumStdoutBytes\":384000,\"maximumStderrBytes\":384000,\"maximumAggregateCaptureBytes\":768000,"
    "\"clientCopies\":{\"maximumBytes\":8388608,\"maximumCopySets\":128,\"release\":\"original-parent-exit\"}}}";
  static const unsigned char duplicate[]="{\"schema\":0,\"\\u0073chema\":1}";
  struct slcli_profile out;
  if(!slcli_profile_parse(body,sizeof(body)-1,&out)||out.raw!=body||
     out.raw_bytes!=sizeof(body)-1||out.record_maximum!=768000||
     out.request_maximum!=16384||out.stdout_maximum!=384000||
     out.stderr_maximum!=384000||out.capture_maximum!=768000) return 0;
  if(slcli_profile_parse(duplicate,sizeof(duplicate)-1,&out)||
     slcli_profile_parse(body,sizeof(body)-2,&out)) return 0;
  return 1;
}
