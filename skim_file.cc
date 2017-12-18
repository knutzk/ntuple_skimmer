#include "TCut.h"
#include "TFile.h"
#include "TKey.h"
#include "TObject.h"
#include "TTree.h"

#include <iostream>
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

  //! Get a list of trees to be processed.
  TreeList get_list_of_trees(const TFile& file);

  TTree* get_tree(TFile* file, const std::string& name);

  //! Check whether object is a tree.
  bool is_tree(const TObject& obj);

  //! Check whether object name contains "nominal".
  bool is_nominal(const TObject& obj);

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
  // std::cout << "Reading file " << argv[1] << std::endl;

  for (const auto& entry : get_list_of_trees(*input_file)) {
    auto tree = get_tree(input_file, entry);
    if (!tree) return -1;

    // std::cout << "  Skimming tree " << tree->GetName() << std::endl;
    output_file->cd();
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
  TreeList get_list_of_trees(const TFile& file) {
    TreeList trees;
    TIter nextkey(file.GetListOfKeys());
    TKey *key = nullptr;
    while ((key = static_cast<TKey*>(nextkey()))) {
      TObject *obj = key->ReadObj();
      if (!is_tree(*obj)) continue;
      // If "copy_weight_trees" is FALSE, we don't want all the
      // systematics garbage. Only collect trees with the keyword
      // "nominal".
      if (!copy_weight_trees && is_nominal(*obj)) continue;
      trees.push_back(obj->GetName());
    }
    return trees;
  }

  TTree* get_tree(TFile* file, const std::string& name) {
    return static_cast<TTree*>(file->Get(name.c_str()));
  }

  bool is_tree(const TObject& obj) {
    return obj.IsA()->InheritsFrom(TTree::Class());
  }

  bool is_nominal(const TObject& obj) {
    return std::string(obj.GetName()).find("nominal") == std::string::npos;
  }

  std::string useMessage(const std::string& prog_name) {
    std::ostringstream msg;
    msg << "Usage: " << prog_name;
    msg << " [input_file]";
    msg << " [output_file]";
    return msg.str();
  }
}  // namespace
