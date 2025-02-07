/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "pixelstats"

#include <android-base/logging.h>
#include <pixelstats/SysfsCollector.h>
#include <pixelstats/UeventListener.h>

#include <thread>

#include <pixelstats/SysfsCollector.h>
#include <pixelstats/UeventListener.h>

using android::hardware::google::pixel::SysfsCollector;
using android::hardware::google::pixel::UeventListener;

#define BLOCK_STATS_LENGTH 17
#define UFSHC_PATH(filename) "/dev/sys/block/bootdevice/" #filename
#define UFS_ERR_PATH(err_type) UFSHC_PATH(err_stats/) #err_type
const struct SysfsCollector::SysfsPaths sysfs_paths = {
    .SlowioReadCntPath = UFSHC_PATH(slowio_read_cnt),
    .SlowioWriteCntPath = UFSHC_PATH(slowio_write_cnt),
    .SlowioUnmapCntPath = UFSHC_PATH(slowio_unmap_cnt),
    .SlowioSyncCntPath = UFSHC_PATH(slowio_sync_cnt),
    .CycleCountBinsPath = "/sys/class/power_supply/battery/cycle_counts",
    .UFSLifetimeA = UFSHC_PATH(health_descriptor/life_time_estimation_a),
    .UFSLifetimeB = UFSHC_PATH(health_descriptor/life_time_estimation_b),
    .UFSLifetimeC = UFSHC_PATH(health_descriptor/life_time_estimation_c),
    .F2fsStatsPath = "/sys/fs/f2fs/",
    .ImpedancePath = "/sys/devices/platform/audiometrics/speaker_impedance",
    .CodecPath =     "/sys/devices/platform/audiometrics/codec_state",
    .EEPROMPath = "/dev/battery_history",
    .MitigationPath = "/sys/devices/virtual/pmic/mitigation",
    .MitigationDurationPath = "/sys/devices/virtual/pmic/mitigation/irq_dur_cnt",
    .BrownoutReasonProp = "vendor.brownout_reason",
    .BrownoutLogPath = "/data/vendor/mitigation/lastmeal.txt",
    .SpeakerTemperaturePath = "/sys/devices/platform/audiometrics/speaker_temp",
    .SpeakerExcursionPath = "/sys/devices/platform/audiometrics/speaker_excursion",
    .SpeakerHeartBeatPath = "/sys/devices/platform/audiometrics/speaker_heartbeat",
    .UFSErrStatsPath = {
        UFS_ERR_PATH(pa_err_count),
        UFS_ERR_PATH(dl_err_count),
        UFS_ERR_PATH(nl_err_count),
        UFS_ERR_PATH(tl_err_count),
        UFS_ERR_PATH(dme_err_count),
        UFS_ERR_PATH(fatal_err_count),
        UFS_ERR_PATH(auto_hibern8_err_count)
    },
    .BlockStatsLength = BLOCK_STATS_LENGTH,
    .AmsRatePath = "/sys/devices/platform/audiometrics/ams_rate_read_once",
    .MitigationPath = "/sys/devices/virtual/pmic/mitigation",
    .ThermalStatsPaths = {
        "/sys/devices/platform/100a0000.BIG/trip_counter",
        "/sys/devices/platform/100a0000.MID/trip_counter",
        "/sys/devices/platform/100a0000.LITTLE/trip_counter",
        "/sys/devices/platform/100b0000.G3D/trip_counter",
        "/sys/devices/platform/100b0000.TPU/trip_counter",
        "/sys/devices/platform/100b0000.AUR/trip_counter",
    },
    .CCARatePath = "/sys/devices/platform/audiometrics/cca_count_read_once",
    .TempResidencyAndResetPaths = {
        {
            "/sys/kernel/metrics/thermal/tr_by_group/tmu/stats",
            "/sys/kernel/metrics/thermal/tr_by_group/tmu/stats_reset"
        },
        {
            "/sys/kernel/metrics/thermal/tr_by_group/spmic/stats",
            "/sys/kernel/metrics/thermal/tr_by_group/spmic/stats_reset"
        }
    },
    .ResumeLatencyMetricsPath = "/sys/kernel/metrics/resume_latency/resume_latency_metrics",
    .LongIRQMetricsPath = "/sys/kernel/metrics/irq/long_irq_metrics",
    .StormIRQMetricsPath = "/sys/kernel/metrics/irq/storm_irq_metrics",
    .IRQStatsResetPath = "/sys/kernel/metrics/irq/stats_reset",
    .ModemPcieLinkStatsPath = "/sys/devices/platform/11920000.pcie/link_stats",
    .WifiPcieLinkStatsPath = "/sys/devices/platform/14520000.pcie/link_stats",
    .GMSRPath = {
        "/sys/class/power_supply/maxfg/gmsr",
        "/sys/class/power_supply/maxfg_base/gmsr",
    },
    .FGModelLoadingPath = {
        "/sys/class/power_supply/maxfg/m5_model_state",
        "/sys/class/power_supply/maxfg_base/m5_model_state"
    },
    .FGLogBufferPath = {
        "/dev/logbuffer_maxfg_monitor",
        "/dev/logbuffer_max77779fg_monitor",
        "/dev/logbuffer_maxfg_base_monitor",
        "/dev/logbuffer_maxfg_secondary_monitor"
    },
    .TotalCallCountPath = "/sys/devices/platform/audiometrics/call_count"
};

const struct UeventListener::UeventPaths ueventPaths = {
        .AudioUevent = "/devices/virtual/amcs/amcs",
        .TypeCPartnerUevent = "PRODUCT_TYPE=",
        .FwUpdatePath = ""
};

int main() {
    LOG(INFO) << "starting PixelStats";

    UeventListener ueventListener(ueventPaths);
    std::thread listenThread(&UeventListener::ListenForever, &ueventListener);
    listenThread.detach();

    SysfsCollector collector(sysfs_paths);
    collector.collect();  // This blocks forever.

    return 0;
}
