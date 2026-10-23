#include "dm.h"
/* Trap probes require a separate Linux-context port. Do not silently accept a
 * diagnostic request while normal game execution does not install any probes. */
void probe_install(void) {
    if(getenv("DM_PROBE") || getenv("DM_PROBE_WATCH"))
        die("[RED] runtime trap probes are not available in the Android platform build");
}
