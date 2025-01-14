/*
  Copyright 2017 SINTEF Digital, Mathematics and Cybernetics.
  Copyright 2017 Statoil ASA.
  Copyright 2017 IRIS
  Copyright 2019 Norce

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OPM_WELLINTERFACE_GENERIC_HEADER_INCLUDED
#define OPM_WELLINTERFACE_GENERIC_HEADER_INCLUDED

#include <opm/input/eclipse/Schedule/Well/Well.hpp>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Opm
{

class DeferredLogger;
class GuideRate;
class ParallelWellInfo;
struct PerforationData;
struct PhaseUsage;
class SummaryState;
class VFPProperties;
class WellTestState;
class WellState;
class SingleWellState;
class GroupState;
class Group;
class Schedule;

class WellInterfaceGeneric {
public:

    WellInterfaceGeneric(const Well& well,
                         const ParallelWellInfo& parallel_well_info,
                         const int time_step,
                         const int pvtRegionIdx,
                         const int num_components,
                         const int num_phases,
                         const int index_of_well,
                         const std::vector<PerforationData>& perf_data);

    /// \brief Get the perforations of the well
    const std::vector<PerforationData>& perforationData() const;

    /// Well name.
    const std::string& name() const;

    /// True if the well is an injector.
    bool isInjector() const;

    /// True if the well is a producer.
    bool isProducer() const;

    /// Well cells.
    const std::vector<int>& cells() const { return well_cells_; }

    /// Index of well in the wells struct and wellState
    int indexOfWell() const;

    void adaptRatesForVFP(std::vector<double>& rates) const;

    const Well& wellEcl() const;
    const PhaseUsage& phaseUsage() const;

    /// Returns true if the well is currently in prediction mode (i.e. not history mode).
    bool underPredictionMode() const;

    // whether the well is operable
    bool isOperableAndSolvable() const;

    void initCompletions();
    void closeCompletions(const WellTestState& wellTestState);

    void setVFPProperties(const VFPProperties* vfp_properties_arg);
    void setGuideRate(const GuideRate* guide_rate_arg);
    void setWellEfficiencyFactor(const double efficiency_factor);
    void setRepRadiusPerfLength();
    void setWsolvent(const double wsolvent);
    void setDynamicThpLimit(const double thp_limit);
    void updatePerforatedCell(std::vector<bool>& is_cell_perforated);

    /// Returns true if the well has one or more THP limits/constraints.
    bool wellHasTHPConstraints(const SummaryState& summaryState) const;

    void stopWell() {
        this->wellStatus_ = Well::Status::STOP;
    }

    void openWell() {
        this->wellStatus_ = Well::Status::OPEN;
    }

    bool wellIsStopped() const {
        return this->wellStatus_ == Well::Status::STOP;
    }

    int currentStep() const {
        return this->current_step_;
    }

    int pvtRegionIdx() const {
        return pvtRegionIdx_;
    }

    const GuideRate* guideRate() const {
        return guide_rate_;
    }

    int numComponents() const {
        return num_components_;
    }

    int numPhases() const {
        return number_of_phases_;
    }

    int numPerfs() const {
        return number_of_perforations_;
    }

    double refDepth() const {
        return ref_depth_;
    }

    double gravity() const {
        return gravity_;
    }

    const VFPProperties* vfpProperties() const {
        return vfp_properties_;
    }

    const ParallelWellInfo& parallelWellInfo() const {
        return parallel_well_info_;
    }

    const std::vector<double>& perfDepth() const {
        return perf_depth_;
    }

    std::vector<double>& perfDepth() {
        return perf_depth_;
    }

    const std::vector<double>& wellIndex() const {
        return well_index_;
    }

    double getTHPConstraint(const SummaryState& summaryState) const;
    double getALQ(const WellState& well_state) const;
    double wsolvent() const;

    // whether a well is specified with a non-zero and valid VFP table number
    bool isVFPActive(DeferredLogger& deferred_logger) const;

    void reportWellSwitching(const SingleWellState& ws, DeferredLogger& deferred_logger) const;

    bool changedToOpenThisStep() const {
        return this->changed_to_open_this_step_;
    }
protected:
    bool getAllowCrossFlow() const;
    double mostStrictBhpFromBhpLimits(const SummaryState& summaryState) const;
    void updateWellTestStatePhysical(const double simulation_time,
                                     const bool write_message_to_opmlog,
                                     WellTestState& well_test_state,
                                     DeferredLogger& deferred_logger) const;


    // definition of the struct OperabilityStatus
    struct OperabilityStatus {
        bool isOperableAndSolvable() const {
            if (!operable_under_only_bhp_limit || !solvable || has_negative_potentials) {
                return false;
            } else {
                return ( (isOperableUnderBHPLimit() || isOperableUnderTHPLimit()) );
            }
        }

        bool isOperableUnderBHPLimit() const {
            return operable_under_only_bhp_limit && obey_thp_limit_under_bhp_limit;
        }

        bool isOperableUnderTHPLimit() const {
            return can_obtain_bhp_with_thp_limit && obey_bhp_limit_with_thp_limit;
        }

        void resetOperability() {
            operable_under_only_bhp_limit = true;
            obey_thp_limit_under_bhp_limit = true;
            can_obtain_bhp_with_thp_limit = true;
            obey_bhp_limit_with_thp_limit = true;
        }

        // whether the well can be operated under bhp limit
        // without considering other limits.
        // if it is false, then the well is not operable for sure.
        bool operable_under_only_bhp_limit = true;
        // if the well can be operated under bhp limit, will it obey(not violate)
        // the thp limit when operated under bhp limit
        bool obey_thp_limit_under_bhp_limit = true;
        // whether the well operate under the thp limit only
        bool can_obtain_bhp_with_thp_limit = true;
        // whether the well obey bhp limit when operated under thp limit
        bool obey_bhp_limit_with_thp_limit = true;
        // the well is solveable
        bool solvable = true;
        // the well have non positive potentials
        bool has_negative_potentials = false;
    };

    OperabilityStatus operability_status_;

    Well well_ecl_;

    const ParallelWellInfo& parallel_well_info_;
    const int current_step_;

    // The pvt region of the well. We assume
    // We assume a well to not penetrate more than one pvt region.
    const int pvtRegionIdx_;

    const int num_components_;

    // number of phases
    int number_of_phases_;

    // the index of well in Wells struct
    int index_of_well_;

    const std::vector<PerforationData>* perf_data_;

    // the vectors used to describe the inflow performance relationship (IPR)
    // Q = IPR_A - BHP * IPR_B
    // TODO: it minght need to go to WellInterface, let us implement it in StandardWell first
    // it is only updated and used for producers for now
    mutable std::vector<double> ipr_a_;
    mutable std::vector<double> ipr_b_;

    // cell index for each well perforation
    std::vector<int> well_cells_;

    // well index for each perforation
    std::vector<double> well_index_;

    // number of the perforations for this well
    int number_of_perforations_;

    // depth for each perforation
    std::vector<double> perf_depth_;

    // representative radius of the perforations, used in shear calculation
    std::vector<double> perf_rep_radius_;

    // length of the perforations, use in shear calculation
    std::vector<double> perf_length_;

    // well bore diameter
    std::vector<double> bore_diameters_;

    /*
     *  completions_ contains the mapping from completion id to connection indices
     *  {
     *      2 : [ConnectionIndex, ConnectionIndex],
     *      1 : [ConnectionIndex, ConnectionIndex, ConnectionIndex],
     *      5 : [ConnectionIndex],
     *      7 : [ConnectionIndex]
     *      ...
     *   }
     *   The integer IDs correspond to the COMPLETION id given by the COMPLUMP keyword.
     *   When there is no COMPLUMP keyword used, a default completion number will be assigned
     *   based on the order of the declaration of the connections.
     *   Since the connections not OPEN is not included in the Wells, so they will not be considered
     *   in this mapping relation.
     */
    std::map<int, std::vector<int>> completions_;

    // reference depth for the BHP
    double ref_depth_;

    // saturation table nubmer for each well perforation
    std::vector<int> saturation_table_number_;

    Well::Status wellStatus_;

    const PhaseUsage* phase_usage_;

    double gravity_;
    double wsolvent_;
    std::optional<double> dynamic_thp_limit_;

    double well_efficiency_factor_;
    const VFPProperties* vfp_properties_;
    const GuideRate* guide_rate_;

    std::vector< std::string> well_control_log_;

    bool changed_to_open_this_step_ = false;
};

}

#endif // OPM_WELLINTERFACE_HEADER_INCLUDED
