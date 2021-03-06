/*
 * Copyright (c) 2020 Pantheon.tech and/or its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <uapi/linux/ptrace.h>

struct data_t
{
    u32  is_del;
    u32  is_ipv6;
    u32  sw_if_index;
    u8   ip_address[16];
    u8   mac_address[6];
    u32  flags;
};

BPF_PERF_OUTPUT(nbh_events);

/**
 * @brief Reads VPP's neighbor updates (ARP / NS), flush it using BPF perf event.
 * @param ctx The BPF context.
 *
 *  DTRACE_PROBE ...
 */
int vpp_nbh_updates(struct pt_regs *ctx) {
    struct data_t data = {};

    bpf_usdt_readarg(1, ctx, &data.is_del);
    bpf_usdt_readarg(2, ctx, &data.is_ipv6);
    bpf_usdt_readarg(3, ctx, &data.sw_if_index);
    bpf_usdt_readarg_p(4, ctx,&data.ip_address, sizeof(data.ip_address));
    bpf_usdt_readarg_p(5, ctx,&data.mac_address, sizeof(data.mac_address));
    bpf_usdt_readarg(6, ctx, &data.flags);

    nbh_events.perf_submit(ctx, &data, sizeof(data));

    return 0;
};