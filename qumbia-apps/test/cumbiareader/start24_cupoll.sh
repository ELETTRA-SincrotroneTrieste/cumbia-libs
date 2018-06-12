#!/bin/bash

LD_LIBRARY_PATH=/usr/local/cumbia-cupoller:$LD_LIBRARY_PATH /usr/local/bin/cumbiareader-cupoller "inj/diagnostics/rtbpm_inj.01/VaPickup" \
"inj/diagnostics/rtbpm_inj.01/VbPickup" \
"inj/diagnostics/rtbpm_inj.01->GetHorPos(0,1000)" \
"inj/diagnostics/rtbpm_inj.01->GetVerPos(0,1000)" \
"inj/diagnostics/rtbpm_inj.01/VcPickup" \
"inj/diagnostics/rtbpm_inj.01/VdPickup" \
l00/diagnostics/rtbpm_l00.01/VaPickup \
l00/diagnostics/rtbpm_l00.01/VbPickup \
"l00/diagnostics/rtbpm_l00.01->GetHorPos(0,1000)" \
"l00/diagnostics/rtbpm_l00.01->GetVerPos(0,1000)" \
l00/diagnostics/rtbpm_l00.01/VcPickup \
l00/diagnostics/rtbpm_l00.01/VdPickup  \
l00/diagnostics/rtbpm_l00.01/VaPickup \
l00/diagnostics/rtbpm_l00.01/VbPickup \
l00/diagnostics/rtbpm_l00.01/VcPickup \
l00/diagnostics/rtbpm_l00.01/VdPickup \
"l00/diagnostics/rtbpm_l00.01->GetHorPos(0,1000)" \
"l00/diagnostics/rtbpm_l00.01->GetVerPos(0,1000)" \
"l00/diagnostics/rtbpm_l00.01->GetSum(0,1000)" \
l01/diagnostics/rtbpm_l01.01/VaPickup \
l01/diagnostics/rtbpm_l01.01/VbPickup \
l01/diagnostics/rtbpm_l01.01/VcPickup \
l01/diagnostics/rtbpm_l01.01/VdPickup \
"l01/diagnostics/rtbpm_l01.01->GetHorPos(0,1000)" \
"l01/diagnostics/rtbpm_l01.01->GetVerPos(0,1000)" \
"l01/diagnostics/rtbpm_l01.01->GetSum(0,1000)" \
\
\
--truncate 10
