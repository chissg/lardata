/**
 * @file   DumpParticleIDs_module.cc
 * @brief  Dump ParticleID objects.
 * @author H. Greenlee
 * @date   Oct. 14, 2021
 */

// C//C++ standard libraries
#include <string>

// support libraries
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Comment.h"

// art libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Utilities/InputTag.h"

// ... plus see below ...

namespace pid {

  /**
   * @brief Prints the content of all the partidle IDs on screen
   *
   * This analyser prints the content of all the particle IDs into the
   * LogInfo/LogVerbatim stream.
   *
   * Configuration parameters
   * =========================
   *
   * - *ParticleIDModuleLabel* (string): label of the producer used to create the
   *   anab::ParticleID collection
   * - *OutputCategory* (string, default: "DumpParticleIDs"): the category
   *   used for the output (useful for filtering)
   *
   */
  class DumpParticleIDs: public art::EDAnalyzer {
      public:

    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;

      fhicl::Atom<art::InputTag> ParticleIDModuleLabel{
        Name("ParticleIDModuleLabel"),
        Comment("tag of the producer used to create the anab::ParticleID collection")
        };

      fhicl::Atom<std::string> OutputCategory{
        Name("OutputCategory"),
        Comment("the messagefacility category used for the output"),
        "DumpParticleIDs"
        };

    }; // Config

    using Parameters = art::EDAnalyzer::Table<Config>;


    /// Default constructor
    explicit DumpParticleIDs(Parameters const& config);

    /// Does the printing
    void analyze (const art::Event& evt) override;

      private:

    art::InputTag const fParticleIDsModuleLabel; ///< name of module that produced the pids
    std::string const fOutputCategory;    ///< category for LogInfo output

  }; // class DumpParticleIDs

} // namespace pid


//------------------------------------------------------------------------------
//---  module implementation
//---
// C//C++ standard libraries
#include <memory> // std::unique_ptr<>

// support libraries
#include "messagefacility/MessageLogger/MessageLogger.h"

// art libraries
#include "art/Framework/Principal/Handle.h"

// LArSoft includes
#include "lardataobj/AnalysisBase/ParticleID.h"


namespace pid {

  //-------------------------------------------------
  DumpParticleIDs::DumpParticleIDs(Parameters const& config)
    : EDAnalyzer              (config)
    , fParticleIDsModuleLabel (config().ParticleIDModuleLabel())
    , fOutputCategory         (config().OutputCategory())
    {}


  //-------------------------------------------------
  void DumpParticleIDs::analyze(const art::Event& evt) {

    // fetch the data to be dumped on screen
    auto const& ParticleIDs = evt.getProduct<std::vector<anab::ParticleID>>(fParticleIDsModuleLabel);

    mf::LogInfo(fOutputCategory)
      << "The event contains " << ParticleIDs.size() << " '"
      << fParticleIDsModuleLabel.encode() << "' particle IDs";

    unsigned int ipid = 0;
    for (const anab::ParticleID& pid: ParticleIDs) {

      // print a header for the cluster
      mf::LogVerbatim log(fOutputCategory);
      log << "ParticleID #" << ipid << '\n';
      log << "Plane ID = " << pid.PlaneID() << '\n';
      auto scores = pid.ParticleIDAlgScores();
      log << "Number of algorithms = " << scores.size() << '\n';
      int ialg = 0;
      for(const anab::sParticleIDAlgScores score: scores) {
        log << "  ParticleID #" << ipid << ", Algorithm " << ialg << '\n'
            << "    Algorithm name = " << score.fAlgName << '\n'
            << "    Variable type  = " << score.fVariableType << '\n'
            << "    TrackDirection = " << score.fTrackDir << '\n'
            << "    NDF            = " << score.fNdf << '\n'
            << "    Assumed PDG    = " << score.fAssumedPdg << '\n'
            << "    Value          = " << score.fValue << '\n'
            << "    Plane Mask     = " << score.fPlaneMask << '\n';
        ++ialg;
      }

      ++ipid;
    } // for pids

  } // DumpParticleIDs::analyze()

  DEFINE_ART_MODULE(DumpParticleIDs)

} // namespace pid
