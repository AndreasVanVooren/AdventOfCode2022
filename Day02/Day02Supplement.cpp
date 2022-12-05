#include "Day02Supplement.h"

#include <vector>
#include <map>
#include <utility>

namespace Day02SupplementInternal { enum class RPSValue : int { Dynamite, Tornado, Quicksand, Pit, Chain, Gun, Law, Whip, Sword, Rock, Death, Wall, Sun, Camera, Fire, Chainsaw, School, Scissors, Poison, Cage, Axe, Peace, Computer, Castle, Snake, Blood, Porcupine, Vulture, Monkey, King, Queen, Prince, Princess, Police, Woman, Baby, Man, Home, Train, Car, Noise, Bicycle, Tree, Turnip, Duck, Wolf, Cat, Bird, Fish, Spider, Cockroach, Brain, Community, Cross, Money, Vampire, Sponge, Church, Butter, Book, Paper, Cloud, Airplane, Moon, Grass, Film, Toilet, Air, Planet, Guitar, Bowl, Cup, Beer, Rain, Water, TV, Rainbow, UFO, Alien, Prayer, Mountain, Satan, Dragon, Diamond, Platinum, Gold, Devil, Fence, VideoGame, Math, Robot, Heart, Electricity, Lightning, Medusa, Power, Laser, Nuke, Sky, Tank, Helicopter, MAX }; constexpr int ScoreForValue(RPSValue val){ switch (val) { case RPSValue::Rock: return 1; case RPSValue::Paper: return 2; case RPSValue::Scissors: return 3; default: return 0; } } enum class RPSResult : int { Loss, Draw, Win, }; constexpr int ScoreForResult(RPSResult val) { return static_cast<int>(val) * 3; } static const std::map<std::string, RPSValue> OriginalValueMapping { {"A", RPSValue::Rock}, {"B", RPSValue::Paper}, {"C", RPSValue::Scissors}, {"X", RPSValue::Rock}, {"Y", RPSValue::Paper}, {"Z", RPSValue::Scissors} }; static const std::map<std::string, RPSResult> ResultMapping { {"X", RPSResult::Loss}, {"Y", RPSResult::Draw}, {"Z", RPSResult::Win} }; constexpr RPSResult GetResultForRHS(RPSValue lhs, RPSValue rhs) { constexpr int max = static_cast<int>(RPSValue::MAX); constexpr int halfMax = max / 2; int intermediate = static_cast<int>(rhs) - static_cast<int>(lhs); while (intermediate > halfMax) { intermediate -= max; } while (intermediate < -halfMax) { intermediate += max; } if (intermediate > 0) { return RPSResult::Loss; } else if (intermediate < 0) { return RPSResult::Win; } else { return RPSResult::Draw; } } static_assert(GetResultForRHS(RPSValue::Rock, RPSValue::Scissors) == RPSResult::Loss); static_assert(GetResultForRHS(RPSValue::Scissors, RPSValue::Paper) == RPSResult::Loss); static_assert(GetResultForRHS(RPSValue::Paper, RPSValue::Rock) == RPSResult::Loss); static_assert(GetResultForRHS(RPSValue::Rock, RPSValue::Paper) == RPSResult::Win); static_assert(GetResultForRHS(RPSValue::Scissors, RPSValue::Rock) == RPSResult::Win); static_assert(GetResultForRHS(RPSValue::Paper, RPSValue::Scissors) == RPSResult::Win); RPSValue GetValueForResult(RPSValue lhs, RPSResult rhs) { switch (rhs) { case RPSResult::Loss: switch (lhs) { case RPSValue::Rock: return RPSValue::Scissors; case RPSValue::Paper: return RPSValue::Rock; case RPSValue::Scissors: return RPSValue::Paper; } break; case RPSResult::Draw: return lhs; case RPSResult::Win: switch (lhs) { case RPSValue::Rock: return RPSValue::Paper; case RPSValue::Paper: return RPSValue::Scissors; case RPSValue::Scissors: return RPSValue::Rock; } break;} return {}; } } std::string Day02Supplement::GetResultStr() { using namespace Day02SupplementInternal; std::stringstream resultStream{}; std::vector<int> scoresPerTurn{}; int scoreTotal{}; const auto initValues = [&] { scoreTotal = 0; scoresPerTurn = {}; }; const auto logResult = [&](const std::string& debug = {}) { if (debug.empty()) { resultStream << "Total result = " << scoreTotal << "\n"; } else { resultStream << "Total result for " << debug << " = " << scoreTotal << "\n"; } }; const auto splitLine = [](const std::string& line, const std::string substr) -> std::pair<std::string, std::string> { const auto offset = line.find(substr); if (offset != line.npos) { return { line.substr(0, offset), line.substr(offset + substr.length()) }; } return std::pair<std::string, std::string>{line, {}}; }; const auto lineLambdaAssumingXYZIsValue = [&scoreTotal, &scoresPerTurn, &splitLine](const std::string& line) { const auto [lhs, rhs] = splitLine(line, " "); const auto lhsIter = OriginalValueMapping.find(lhs); const auto rhsIter = OriginalValueMapping.find(rhs); if (lhsIter != OriginalValueMapping.end() && rhsIter != OriginalValueMapping.end()) { const RPSResult result = GetResultForRHS(lhsIter->second, rhsIter->second); const int scoreTurn = ScoreForValue(rhsIter->second) + ScoreForResult(result); scoreTotal += scoreTurn; scoresPerTurn.push_back(scoreTurn); } }; const auto lineLambdaAssumingXYZIsResult = [&scoreTotal, &scoresPerTurn, &splitLine](const std::string& line) { const auto [lhs, rhs] = splitLine(line, " "); const auto lhsIter = OriginalValueMapping.find(lhs); const auto rhsIter = ResultMapping.find(rhs); if (lhsIter != OriginalValueMapping.end() && rhsIter != ResultMapping.end()) { const RPSValue counterValue = GetValueForResult(lhsIter->second, rhsIter->second); const int scoreTurn = ScoreForResult(rhsIter->second) + ScoreForValue(counterValue); scoreTotal += scoreTurn; scoresPerTurn.push_back(scoreTurn); } }; initValues(); ForEachLineInTestInputFile(lineLambdaAssumingXYZIsValue); logResult("XYZ = value"); initValues(); ForEachLineInInputFile(lineLambdaAssumingXYZIsValue); logResult("XYZ = value"); initValues(); ForEachLineInTestInputFile(lineLambdaAssumingXYZIsResult); logResult("XYZ = result"); initValues(); ForEachLineInInputFile(lineLambdaAssumingXYZIsResult); logResult("XYZ = result"); return resultStream.str(); } std::string Day02Supplement::GetIdStr() { return "Day02"; }