/* Copyright (C) Jelle Spijker - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * and only allowed with the written consent of the author (Jelle Spijker)
 * This software is proprietary and confidential
 * Written by Jelle Spijker <spijker.jelle@gmail.com>, 2015
 */

#include "NN.h"

namespace SoilMath {
NN::NN() { beta = 0.666; }

NN::NN(uint32_t inputneurons, uint32_t hiddenneurons, uint32_t outputneurons) {
  // Set the number of neurons in the network
  inputNeurons = inputneurons;
  hiddenNeurons = hiddenneurons;
  outputNeurons = outputneurons;
  // Reserve the vector space
  iNeurons.reserve(inputNeurons + 1);  // input neurons + bias
  hNeurons.reserve(hiddenNeurons + 1); // hidden neurons + bias
  oNeurons.reserve(outputNeurons);     // output neurons

  beta = 0.666;
}

NN::~NN() {}

void NN::LoadState(string filename) {
  std::ifstream ifs(filename.c_str());
  boost::archive::xml_iarchive ia(ifs);
  ia >> boost::serialization::make_nvp("NeuralNet", *this);
}

void NN::SaveState(string filename) {
  std::ofstream ofs(filename.c_str());
  boost::archive::xml_oarchive oa(ofs);
  oa << boost::serialization::make_nvp("NeuralNet", *this);
}

Predict_t NN::PredictLearn(ComplexVect_t input, Weight_t inputweights,
                           Weight_t hiddenweights, uint32_t inputneurons,
                           uint32_t hiddenneurons, uint32_t outputneurons) {
  NN neural(inputneurons, hiddenneurons, outputneurons);
  neural.SetInputWeights(inputweights);
  neural.SetHiddenWeights(hiddenweights);
  return neural.Predict(input);
}

Predict_t NN::Predict(ComplexVect_t input) {
  if (input.size() != inputNeurons) {
    throw Exception::MathException("Size of input Neurons Exception!");
  }

  iNeurons.clear();
  hNeurons.clear();
  oNeurons.clear();

  // Set the bias in the input and hidden vector to 1 (real number)
  iNeurons.push_back(1.0f);
  hNeurons.push_back(1.0f);

  Predict_t retVal;
  uint32_t wCount = 0;

  // Init the network
  for (uint32_t i = 0; i < inputNeurons; i++) {
    iNeurons.push_back(static_cast<float>(abs(input[i])));
  }
  for (uint32_t i = 0; i < hiddenNeurons; i++) {
    hNeurons.push_back(0.0f);
  }
  for (uint32_t i = 0; i < outputNeurons; i++) {
    oNeurons.push_back(0.0f);
  }

  for (uint32_t i = 1; i < hNeurons.size(); i++) {
    wCount = i - 1;
    for (uint32_t j = 0; j < iNeurons.size(); j++) {
      hNeurons[i] += iNeurons[j] * iWeights[wCount];
      wCount += hNeurons.size() - 1;
    }
    hNeurons[i] = 1 / (1 + pow(2.71828f, (-hNeurons[i] * beta)));
  }

  for (uint32_t i = 0; i < oNeurons.size(); i++) {
    wCount = i;
    for (uint32_t j = 0; j < hNeurons.size(); j++) {
      oNeurons[i] += hNeurons[j] * hWeights[wCount];
      wCount += oNeurons.size();
    }
    oNeurons[i] =
        (2 / (1.0f + pow(2.71828f, (-oNeurons[i] * beta)))) -
        1; // Shift plus scale so the learning function can be calculated
  }

  retVal.OutputNeurons = oNeurons;
  return retVal;
}

void NN::Learn(InputLearnVector_t input, OutputLearnVector_t cat,
               uint32_t noOfDescriptorsUsed) {
  SoilMath::GA optim(PredictLearn, inputNeurons, hiddenNeurons, outputNeurons);
  ComplexVect_t inputTest;
  std::vector<Weight_t> weights;
  Weight_t weight(((inputNeurons + 1) * hiddenNeurons) +
                      ((hiddenNeurons + 1) * outputNeurons),
                  0);
  // loop through each case and adjust the weights
  optim.Evolve(input, weight, MinMaxWeight_t(-50, 50), cat, 1000, 50);

  this->iWeights = Weight_t(
      weight.begin(), weight.begin() + ((inputNeurons + 1) * hiddenNeurons));
  this->hWeights = Weight_t(
      weight.begin() + ((inputNeurons + 1) * hiddenNeurons), weight.end());
  studied = true;
}
}
