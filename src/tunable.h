/*
 * oranj, a UCI shatranj engine
 * Copyright (C) 2025 Ciekce
 *
 * oranj is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * oranj is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oranj. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.h"

#include <string>
#include <array>
#include <functional>

#include "util/range.h"
#include "util/multi_array.h"

#ifndef OJ_EXTERNAL_TUNE
	#define OJ_EXTERNAL_TUNE 1
#endif

namespace oranj::tunable
{
	auto init() -> void;

	// [noisy][depth][legal moves]
	extern util::MultiArray<i32, 2, 256, 256> g_lmrTable;

	auto updateQuietLmrTable() -> void;
	auto updateNoisyLmrTable() -> void;

#define OJ_TUNABLE_ASSERTS(Default, Min, Max, Step) \
	static_assert((Default) >= (Min)); \
	static_assert((Default) <= (Max)); \
	static_assert((Min) < (Max)); \
	static_assert((Min) + (Step) <= Max); \
	static_assert((Step) >= 0.5);

#define OJ_TUNABLE_ASSERTS_F64(Default, Min, Max, Step, Q) \
	OJ_TUNABLE_ASSERTS(static_cast<i32>((Default) * (Q)), static_cast<i32>((Min) * (Q)), \
		static_cast<i32>((Max) * (Q)), static_cast<i32>((Step) * (Q))) \
    static_assert((Q) > 0); \
    static_assert(static_cast<f64>(static_cast<i32>(Q)) == Q);

#if OJ_EXTERNAL_TUNE
	struct TunableParam
	{
		std::string name;
		std::string lowerName;
		i32 defaultValue;
		i32 value;
		util::Range<i32> range;
		f64 step;
		std::function<void()> callback;
	};

	auto addTunableParam(const std::string &name, i32 value,
		i32 min, i32 max, f64 step, std::function<void()> callback) -> TunableParam &;

	#define OJ_TUNABLE_PARAM(Name, Default, Min, Max, Step) \
	    OJ_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		inline TunableParam &param_##Name = addTunableParam(#Name, Default, Min, Max, Step, nullptr); \
		inline auto Name() { return param_##Name.value; }

	#define OJ_TUNABLE_PARAM_CALLBACK(Name, Default, Min, Max, Step, Callback) \
	    OJ_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		inline TunableParam &param_##Name = addTunableParam(#Name, Default, Min, Max, Step, Callback); \
		inline auto Name() { return param_##Name.value; }

	//TODO tune these as actual floating-point values
	#define OJ_TUNABLE_PARAM_F64(Name, Default, Min, Max, Step, Q) \
	    OJ_TUNABLE_ASSERTS_F64(Default, Min, Max, Step, Q) \
		inline TunableParam &param_##Name = addTunableParam(#Name, \
			(Default) * (Q), (Min) * (Q), (Max) * (Q), (Step) * (Q), nullptr); \
		inline auto Name() { return static_cast<f64>(param_##Name.value) / (Q); }
#else
	#define OJ_TUNABLE_PARAM(Name, Default, Min, Max, Step) \
		OJ_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		constexpr auto Name() -> i32 { return Default; }
	#define OJ_TUNABLE_PARAM_CALLBACK(Name, Default, Min, Max, Step, Callback) \
		OJ_TUNABLE_PARAM(Name, Default, Min, Max, Step)

#define OJ_TUNABLE_PARAM_F64(Name, Default, Min, Max, Step, Q) \
		OJ_TUNABLE_ASSERTS_F64(Default, Min, Max, Step, Q) \
		constexpr auto Name() -> f64 { return Default; }
#endif

	OJ_TUNABLE_PARAM(defaultMovesToGo, 19, 12, 40, 1)
	OJ_TUNABLE_PARAM_F64(incrementScale, 0.83, 0.5, 1.0, 0.05, 100)
	OJ_TUNABLE_PARAM_F64(softTimeScale, 0.68, 0.5, 1.0, 0.05, 100)
	OJ_TUNABLE_PARAM_F64(hardTimeScale, 0.56, 0.2, 1.0, 0.05, 100)

	OJ_TUNABLE_PARAM_F64(nodeTmBase, 2.63, 1.5, 3.0, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(nodeTmScale, 1.7, 1.0, 2.5, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(nodeTmScaleMin, 0.102, 0.001, 1.0, 0.1, 1000)

	OJ_TUNABLE_PARAM_F64(bmStabilityTmMin, 0.75, 0.4, 1.0, 0.03, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmMax, 2.4, 1.2, 10.0, 0.4, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmScale, 9.11, 2.0, 15.0, 0.65, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmOffset, 0.8, 0.5, 2.0, 0.08, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmPower, -2.7, -4.0, -1.5, 0.13, 100)

	OJ_TUNABLE_PARAM_F64(scoreTrendTmMin, 0.6, 0.4, 1.0, 0.03, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmMax, 1.7, 1.2, 10.0, 0.4, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmScoreScale, 4.58, 0.1, 10.0, 0.5, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmStretch, 0.8, 0.1, 2.0, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmScale, 0.41, 0.1, 0.9, 0.04, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmPositiveScale, 1.09, 0.5, 2.0, 0.075, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmNegativeScale, 1.04, 0.5, 2.0, 0.075, 100)

	OJ_TUNABLE_PARAM_F64(timeScaleMin, 0.07, 0.001, 1.0, 0.1, 1000)

	OJ_TUNABLE_PARAM(pawnCorrhistWeight, 120, 32, 384, 18)
	OJ_TUNABLE_PARAM(stmNonPawnCorrhistWeight, 136, 32, 384, 18)
	OJ_TUNABLE_PARAM(nstmNonPawnCorrhistWeight, 143, 32, 384, 18)
	OJ_TUNABLE_PARAM(majorCorrhistWeight, 106, 32, 384, 18)
	OJ_TUNABLE_PARAM(contCorrhistWeight, 128, 32, 384, 18)

	OJ_TUNABLE_PARAM(initialAspWindow, 10, 4, 50, 4)
	OJ_TUNABLE_PARAM(aspWideningFactor, 17, 1, 24, 1)

	OJ_TUNABLE_PARAM(goodNoisySeeOffset, 3, -384, 384, 40)

	OJ_TUNABLE_PARAM(rfpMargin, 70, 25, 150, 5)

	OJ_TUNABLE_PARAM(razoringMargin, 305, 100, 350, 40)

	OJ_TUNABLE_PARAM(nmpEvalReductionScale, 208, 50, 300, 25)

	OJ_TUNABLE_PARAM(probcutMargin, 297, 150, 400, 13)
	OJ_TUNABLE_PARAM(probcutSeeScale, 17, 6, 24, 1)

	OJ_TUNABLE_PARAM(fpMargin, 230, 120, 350, 45)
	OJ_TUNABLE_PARAM(fpScale, 64, 40, 80, 8)

	OJ_TUNABLE_PARAM(quietHistPruningMargin, -2342, -4000, -1000, 175)
	OJ_TUNABLE_PARAM(quietHistPruningOffset, -1048, -4000, 4000, 400)

	OJ_TUNABLE_PARAM(noisyHistPruningMargin, -1054, -4000, -1000, 175)
	OJ_TUNABLE_PARAM(noisyHistPruningOffset, -941, -4000, 4000, 400)

	OJ_TUNABLE_PARAM(seePruningThresholdQuiet, -19, -80, -15, 12)
	OJ_TUNABLE_PARAM(seePruningThresholdNoisy, -116, -120, -40, 20)

	OJ_TUNABLE_PARAM(sBetaMargin, 14, 4, 64, 12)

	OJ_TUNABLE_PARAM(multiExtLimit, 9, 4, 24, 4)

	OJ_TUNABLE_PARAM(doubleExtMargin, 11, 0, 32, 5)
	OJ_TUNABLE_PARAM(tripleExtMargin, 105, 10, 150, 7)

	OJ_TUNABLE_PARAM_CALLBACK(quietLmrBase, 101, 50, 120, 15, updateQuietLmrTable)
	OJ_TUNABLE_PARAM_CALLBACK(quietLmrDivisor, 212, 100, 300, 10, updateQuietLmrTable)

	OJ_TUNABLE_PARAM_CALLBACK(noisyLmrBase, -20, -50, 75, 10, updateNoisyLmrTable)
	OJ_TUNABLE_PARAM_CALLBACK(noisyLmrDivisor, 256, 150, 350, 10, updateNoisyLmrTable)

	OJ_TUNABLE_PARAM(lmrNonPvReductionScale, 134, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrTtpvReductionScale, 128, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrImprovingReductionScale, 144, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrCheckReductionScale, 106, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrCutnodeReductionScale, 249, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrHighComplexityReductionScale, 123, 32, 384, 12)

	OJ_TUNABLE_PARAM(lmrHistoryDivisor, 10830, 4096, 16384, 650)

	OJ_TUNABLE_PARAM(lmrHighComplexityThreshold, 69, 30, 120, 5)

	OJ_TUNABLE_PARAM(lmrDeeperBase, 33, 20, 100, 6)
	OJ_TUNABLE_PARAM(lmrDeeperScale, 5, 3, 12, 1)

	OJ_TUNABLE_PARAM(maxHistory, 15683, 8192, 32768, 256)

	OJ_TUNABLE_PARAM(maxHistoryBonus, 2488, 1024, 4096, 256)
	OJ_TUNABLE_PARAM(historyBonusDepthScale, 276, 128, 512, 32)
	OJ_TUNABLE_PARAM(historyBonusOffset, 372, 128, 768, 64)

	OJ_TUNABLE_PARAM(maxHistoryPenalty, 1126, 1024, 4096, 256)
	OJ_TUNABLE_PARAM(historyPenaltyDepthScale, 360, 128, 512, 32)
	OJ_TUNABLE_PARAM(historyPenaltyOffset, 164, 128, 768, 64)

	OJ_TUNABLE_PARAM(qsearchFpMargin, 148, 50, 400, 17)
	OJ_TUNABLE_PARAM(qsearchSeeThreshold, -108, -200, 200, 20)

#undef OJ_TUNABLE_PARAM
#undef OJ_TUNABLE_PARAM_CALLBACK
#undef OJ_TUNABLE_ASSERTS
}
