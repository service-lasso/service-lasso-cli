#ifndef SLCLI_INPUT_CONSUME_H
#define SLCLI_INPUT_CONSUME_H
#include "input_body.h"
#include "profile.h"
#include "grant.h"
#include "calibration.h"
#include "actors19.h"
#if defined(_WIN32) || defined(__linux__)
/* Internal PA1 consumption of original retained input bodies, never an issuer,
 * enrollment API, ROOT admission or owner factory. Output belongs to the same
 * precharged owner and raw spans retain their original input-body lifetime. */
int slcli_input_profile_consume(struct slcli_input_body *,struct slcli_profile *);
int slcli_input_grant_consume(struct slcli_input_body *,
                              const struct slcli_input_body *,struct slcli_grant *);
int slcli_input_qualification_consume(struct slcli_input_body *,
                                      const struct slcli_input_body *,
                                      struct slcli_qualification *);
/* Separate HQ1 service qualification consumer, using own SAME raw P. This
 * never aliases runtime domain0/2 Q32 or supplies C22/source authority. */
int slcli_input_ha2_service_qualification_consume(struct slcli_input_body *,
  const struct slcli_input_body *,struct slcli_qualification *);
/* T2 retains fifteen independently admitted originals. This second native
 * read joins the public facade image to own P, without granting role13,
 * source12, a process birth or an installed native entry. */
int slcli_input_actors19_consume(struct slcli_input_body *,
  const struct slcli_input_body *,const struct slcli_profile *,struct slcli_actors19 *);
int slcli_input_calibration_consume(struct slcli_input_body *,
                                    const struct slcli_input_body *,
                                    struct slcli_calibration *);
#endif
#endif
