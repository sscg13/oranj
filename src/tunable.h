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
	#define OJ_EXTERNAL_TUNE 0
#endif

namespace oranj::tunable
{
	auto init() -> void;

	// [noisy][depth][legal moves]
	extern util::MultiArray<i32, 2, 256, 256> g_lmrTable;

	// [coloured piece], +1 for none
	extern std::array<i32, 13> g_seeValues;

	auto updateQuietLmrTable() -> void;
	auto updateNoisyLmrTable() -> void;
	auto updateSeeValueTable() -> void;

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
	OJ_TUNABLE_PARAM_F64(incrementScale, 0.84, 0.5, 1.0, 0.05, 100)
	OJ_TUNABLE_PARAM_F64(softTimeScale, 0.69, 0.5, 1.0, 0.05, 100)
	OJ_TUNABLE_PARAM_F64(hardTimeScale, 0.56, 0.2, 1.0, 0.05, 100)

	OJ_TUNABLE_PARAM_F64(nodeTmBase, 2.62, 1.5, 3.0, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(nodeTmScale, 1.67, 1.0, 2.5, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(nodeTmScaleMin, 0.046, 0.001, 1.0, 0.1, 1000)

	OJ_TUNABLE_PARAM_F64(bmStabilityTmMin, 0.74, 0.4, 1.0, 0.03, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmMax, 2.55, 1.2, 10.0, 0.4, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmScale, 9.17, 2.0, 15.0, 0.65, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmOffset, 0.81, 0.5, 2.0, 0.08, 100)
	OJ_TUNABLE_PARAM_F64(bmStabilityTmPower, -2.66, -4.0, -1.5, 0.13, 100)

	OJ_TUNABLE_PARAM_F64(scoreTrendTmMin, 0.59, 0.4, 1.0, 0.03, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmMax, 1.61, 1.2, 10.0, 0.4, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmScoreScale, 4.87, 0.1, 10.0, 0.5, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmStretch, 0.82, 0.1, 2.0, 0.1, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmScale, 0.42, 0.1, 0.9, 0.04, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmPositiveScale, 1.10, 0.5, 2.0, 0.075, 100)
	OJ_TUNABLE_PARAM_F64(scoreTrendTmNegativeScale, 1.02, 0.5, 2.0, 0.075, 100)

	OJ_TUNABLE_PARAM_F64(timeScaleMin, 0.081, 0.001, 1.0, 0.1, 1000)

	OJ_TUNABLE_PARAM_CALLBACK(seeValuePawn, 105, 50, 200, 7.5, updateSeeValueTable)
	OJ_TUNABLE_PARAM_CALLBACK(seeValueAlfil, 131, 50, 200, 7.5, updateSeeValueTable)
	OJ_TUNABLE_PARAM_CALLBACK(seeValueFerz, 156, 100, 350, 15, updateSeeValueTable)
	OJ_TUNABLE_PARAM_CALLBACK(seeValueKnight, 333, 250, 600, 25, updateSeeValueTable)
	OJ_TUNABLE_PARAM_CALLBACK(seeValueRook, 507, 400, 1000, 30, updateSeeValueTable)

	OJ_TUNABLE_PARAM(scalingValuePawn, -3, -20, 200, 15)
	OJ_TUNABLE_PARAM(scalingValueAlfil, 125, 50, 200, 7.5)
	OJ_TUNABLE_PARAM(scalingValueFerz, 161, 100, 350, 15)
	OJ_TUNABLE_PARAM(scalingValueKnight, 331, 250, 600, 25)
	OJ_TUNABLE_PARAM(scalingValueRook, 517, 400, 1000, 30)

	OJ_TUNABLE_PARAM(materialScalingBase, 13256, 5000, 20000, 750)

	OJ_TUNABLE_PARAM(pawnCorrhistWeight, 113, 32, 384, 18)
	OJ_TUNABLE_PARAM(stmNonPawnCorrhistWeight, 116, 32, 384, 18)
	OJ_TUNABLE_PARAM(nstmNonPawnCorrhistWeight, 128, 32, 384, 18)
	OJ_TUNABLE_PARAM(majorCorrhistWeight, 82, 32, 384, 18)
	OJ_TUNABLE_PARAM(contCorrhistWeight, 115, 32, 384, 18)

	OJ_TUNABLE_PARAM(initialAspWindow, 11, 4, 50, 4)
	OJ_TUNABLE_PARAM(aspWideningFactor, 17, 1, 24, 1)

	OJ_TUNABLE_PARAM(goodNoisySeeOffset, 10, -384, 384, 40)

	OJ_TUNABLE_PARAM(rfpMargin, 69, 25, 150, 5)

	OJ_TUNABLE_PARAM(razoringMargin, 310, 100, 350, 40)

	OJ_TUNABLE_PARAM(nmpEvalReductionScale, 209, 50, 300, 25)

	OJ_TUNABLE_PARAM(probcutMargin, 297, 150, 400, 13)
	OJ_TUNABLE_PARAM(probcutSeeScale, 17, 6, 24, 1)

	OJ_TUNABLE_PARAM(fpMargin, 235, 120, 350, 45)
	OJ_TUNABLE_PARAM(fpScale, 64, 40, 80, 8)

	OJ_TUNABLE_PARAM(quietHistPruningMargin, -2367, -4000, -1000, 175)
	OJ_TUNABLE_PARAM(quietHistPruningOffset, -1082, -4000, 4000, 400)

	OJ_TUNABLE_PARAM(noisyHistPruningMargin, -1113, -4000, -1000, 175)
	OJ_TUNABLE_PARAM(noisyHistPruningOffset, -869, -4000, 4000, 400)

	OJ_TUNABLE_PARAM(seePruningThresholdQuiet, -9, -80, -5, 12)
	OJ_TUNABLE_PARAM(seePruningThresholdNoisy, -108, -120, -40, 20)

	OJ_TUNABLE_PARAM(sBetaMargin, 4, 4, 64, 12)

	OJ_TUNABLE_PARAM(multiExtLimit, 10, 4, 24, 4)

	OJ_TUNABLE_PARAM(doubleExtMargin, 8, 0, 32, 5)
	OJ_TUNABLE_PARAM(tripleExtMargin, 107, 10, 150, 7)

	OJ_TUNABLE_PARAM_CALLBACK(quietLmrBase, 121, 50, 150, 15, updateQuietLmrTable)
	OJ_TUNABLE_PARAM_CALLBACK(quietLmrDivisor, 213, 100, 300, 10, updateQuietLmrTable)

	OJ_TUNABLE_PARAM_CALLBACK(noisyLmrBase, -25, -50, 75, 10, updateNoisyLmrTable)
	OJ_TUNABLE_PARAM_CALLBACK(noisyLmrDivisor, 254, 150, 350, 10, updateNoisyLmrTable)

	OJ_TUNABLE_PARAM(lmrNonPvReductionScale, 147, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrTtpvReductionScale, 126, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrImprovingReductionScale, 145, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrCheckReductionScale, 109, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrCutnodeReductionScale, 252, 32, 384, 12)
	OJ_TUNABLE_PARAM(lmrHighComplexityReductionScale, 124, 32, 384, 12)

	OJ_TUNABLE_PARAM(lmrQuietHistoryDivisor, 11081, 4096, 16384, 650)
	OJ_TUNABLE_PARAM(lmrNoisyHistoryDivisor, 10701, 4096, 16384, 650)

	OJ_TUNABLE_PARAM(lmrHighComplexityThreshold, 69, 30, 120, 5)

	OJ_TUNABLE_PARAM(lmrDeeperBase, 31, 20, 100, 6)
	OJ_TUNABLE_PARAM(lmrDeeperScale, 5, 3, 12, 1)

	OJ_TUNABLE_PARAM(maxHistory, 15709, 8192, 32768, 256)

	OJ_TUNABLE_PARAM(maxHistoryBonus, 2558, 1024, 4096, 256)
	OJ_TUNABLE_PARAM(historyBonusDepthScale, 254, 128, 512, 32)
	OJ_TUNABLE_PARAM(historyBonusOffset, 383, 128, 768, 64)

	OJ_TUNABLE_PARAM(maxHistoryPenalty, 1361, 1024, 4096, 256)
	OJ_TUNABLE_PARAM(historyPenaltyDepthScale, 378, 128, 512, 32)
	OJ_TUNABLE_PARAM(historyPenaltyOffset, 145, 128, 768, 64)

	OJ_TUNABLE_PARAM(qsearchFpMargin, 142, 50, 400, 17)
	OJ_TUNABLE_PARAM(qsearchSeeThreshold, -96, -200, 200, 20)

#undef OJ_TUNABLE_PARAM
#undef OJ_TUNABLE_PARAM_CALLBACK
#undef OJ_TUNABLE_ASSERTS
}
