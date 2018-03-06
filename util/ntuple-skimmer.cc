#include "TCut.h"
#include "TFile.h"
#include "TKey.h"
#include "TObject.h"
#include "TTree.h"

#include <iostream>
#include <set>
#include <sstream>
#include <string>

//! Set whether to copy weight trees as well (otherwise only copy nominal).
bool copy_weight_trees{false};

//! Cut strings for (1) the hadron-fake CR, (2) the prompt CR.
std::string hfake_cuts{"( selph_index1 >=0 && ( ((ejets_2015 || ejets_2016) && abs(ph_mgammalept[selph_index1] - 91188) > 5000) || (mujets_2015 || mujets_2016) ) && event_ngoodphotons==1 &&  event_njets >= 4 && event_nbjets77 >= 1 && ph_drlept[selph_index1] > 1.0 && ph_ptcone20[selph_index1] > 3000. && !ph_isoFCT[selph_index1] )"};
std::string prompt_cuts{"( selph_index1 >=0 && (ee_2015 || ee_2016 || emu_2015 || emu_2016 || mumu_2015 || mumu_2016) && event_ngoodphotons==1 && ph_drlept[selph_index1] > 1.0 && ph_isoFCT[selph_index1] && (event_mll > 60000 && event_mll < 100000) )"};

//! The final cut string to be applied for skimming.
TCut cut_string = std::string(hfake_cuts + " || " + prompt_cuts).c_str();


// =========================================================
// =========================================================


namespace {
  using TreeList = std::vector<std::string>;
  using BranchSet = std::set<std::string>;

  //! Get a list of trees to be processed.
  TreeList getListOfTrees(const TFile& file);

  //! Get tree 'name' from a TFile object.
  TTree* getTree(TFile* file, const std::string& name);

  //! Remove unwanted branches from a tree.
  void activateBranches(TTree* tree, const BranchSet& branches);

  //! Show me how to use this program.
  std::string useMessage(const std::string& prog_name);
}  // namespace


// =========================================================
// =========================================================


int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << useMessage(argv[0]) << std::endl;
    return -1;
  }

  auto input_file = TFile::Open(argv[1], "READ");
  auto output_file = TFile::Open(argv[2], "RECREATE");
  if (!input_file || !output_file) return -1;

  for (const auto& entry : getListOfTrees(*input_file)) {
    auto tree = getTree(input_file, entry);
    if (!tree) return -1;
    output_file->cd();

    BranchSet branches{};
    // Branches necessary for the event selection.
    branches.insert("ee*");
    branches.insert("ejets*");
    branches.insert("emu*");
    branches.insert("event_mll");
    branches.insert("event_nbjets77");
    branches.insert("event_ngoodphotons");
    branches.insert("event_njets");
    branches.insert("mujets*");
    branches.insert("mumu*");
    branches.insert("ph_drlept");
    branches.insert("ph_isoFCT");
    branches.insert("ph_mgammalept");
    branches.insert("ph_ptcone20");
    branches.insert("selph_index1");

    activateBranches(tree, branches);

    tree->CopyTree(cut_string);
    delete tree;
  }

  input_file->Close();
  output_file->Write();
  output_file->Close();
  return 0;
}


// =========================================================
// =========================================================


namespace {
  //! Check whether TObject is of type TTree.
  bool isTree(const TObject& obj) {
    return obj.IsA()->InheritsFrom(TTree::Class());
  }

  //! Check whether TObject's name contains string 'nominal'.
  bool isNominal(const TObject& obj) {
    return std::string(obj.GetName()).find("nominal") != std::string::npos;
  }

  TreeList getListOfTrees(const TFile& file) {
    TreeList trees;
    TIter nextkey(file.GetListOfKeys());
    TKey *key = nullptr;
    while ((key = static_cast<TKey*>(nextkey()))) {
      TObject *obj = key->ReadObj();
      if (!isTree(*obj)) continue;
      // If "copy_weight_trees" is FALSE, we don't want all the
      // systematics garbage. Only collect trees with the keyword
      // "nominal".
      if (!copy_weight_trees && !isNominal(*obj)) continue;
      trees.push_back(obj->GetName());
    }
    return trees;
  }

  TTree* getTree(TFile* file, const std::string& name) {
    return static_cast<TTree*>(file->Get(name.c_str()));
  }

  void activateBranches(TTree* tree, const BranchSet& branches) {
    tree->SetBranchStatus("*", 0);
    for (const auto& branch : branches) {
      tree->SetBranchStatus(branch.c_str(), 1);
    }
  }

  std::string useMessage(const std::string& prog_name) {
    std::ostringstream msg;
    msg << "Usage: " << prog_name;
    msg << " [input_file]";
    msg << " [output_file]";
    return msg.str();
  }
}  // namespace
