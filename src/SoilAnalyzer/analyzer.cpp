/* Copyright (C) Jelle Spijker - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * and only allowed with the written consent of the author (Jelle Spijker)
 * This software is proprietary and confidential
 * Written by Jelle Spijker <spijker.jelle@gmail.com>, 2015
 */

#include "analyzer.h"

namespace SoilAnalyzer {

/*!
*\brief Analyzer::Analyzer
*\param snapshots
*\param results
*\param settings
*/
Analyzer::Analyzer(Images_t *snapshots, Sample *results,
                   SoilSettings *settings = nullptr) {
  this->Snapshots = snapshots;
  this->Results = results;
  if (settings == nullptr) {
    Settings = new SoilSettings;
  } else {
    this->Settings = settings;
  }
  NeuralNet.LoadState(Settings->NNlocation);
}

/*!
 * \brief Analyzer::PrepImages
 */
void Analyzer::PrepImages() {
  if (Snapshots == nullptr || Snapshots->size() == 0) {
    throw Exception::SoilAnalyzerException(EXCEPTION_NO_SNAPSHOTS,
                                           EXCEPTION_NO_SNAPSHOTS_NR);
  }

  std::vector<cv::Mat> intensityVector;
  GetEnhancedInt(Snapshots, intensityVector);

  std::vector<cv::Mat> BWVector;
  GetBW(intensityVector, BWVector);
  CleanUpMatVector(intensityVector);

  GetParticles(BWVector, Snapshots, Results->ParticlePopulation);

  CleanUpMatVector(BWVector);
  CleanUpMatVector(Snapshots);

  Results->isPreparedForAnalysis = true;
}

void Analyzer::Analyse(Images_t *snapshots, Sample *results,
                       SoilSettings *settings) {
  Snapshots = snapshots;
  Results = results;
  Settings = settings;
  Analyse();
}

/*!
 * \brief Analyzer::Analyse
 */
void Analyzer::Analyse() {
  CalcMaxProgress();
  if (!Results->isPreparedForAnalysis && !Results->IsLoadedFromDisk) {
    PrepImages();
  }
  GetFFD(Results->ParticlePopulation);
  if (PredictShape && Settings->PredictTheShape) {
    GetPrediction(Results->ParticlePopulation);
  }

  Results->Angularity =
      ucharStat_t(Results->GetAngularityVector()->data(),
                  Results->GetAngularityVector()->size(), 1, 7, 0, true);
  emit on_progressUpdate(currentProgress++);

  Results->Roundness =
      ucharStat_t(Results->GetRoundnessVector()->data(),
                  Results->GetRoundnessVector()->size(), 1, 5, 0, true);

  Results->PSD =
      SoilMath::PSD(Results->GetPSDVector()->data(),
                    Results->GetPSDVector()->size(), BinRanges, 15, 14);
  emit on_progressUpdate(currentProgress++);

  emit on_AnalysisFinished();
}

void Analyzer::CleanUpMatVector(std::vector<Mat> &mv) {
  for_each(mv.begin(), mv.end(), [](cv::Mat &I) { I.release(); });
  mv.clear();
}

/*!
 * \brief Analyzer::CleanUpMatVector
 * \param mv
 */
void Analyzer::CleanUpMatVector(Images_t *mv) {
  for_each(mv->begin(), mv->end(), [](Image_t &I) {
    I.BackLight.release();
    I.FrontLight.release();
  });
  mv->clear();
}

/*!
 * \brief Analyzer::CalcMaxProgress
 */
void Analyzer::CalcMaxProgress() {
  // Static processing steps
  MaxProgress += Snapshots->size() * 5;

  // Optional processing steps
  if (Settings->useBlur) {
    MaxProgress += Snapshots->size();
  }
  if (Settings->useAdaptiveContrast) {
    MaxProgress += Snapshots->size();
  }
  if (Settings->fillHoles) {
    MaxProgress += Snapshots->size();
  }
  if (Settings->ignorePartialBorderParticles) {
    MaxProgress += Snapshots->size();
  }
  if (Settings->morphFilterType != Vision::MorphologicalFilter::NONE) {
    MaxProgress += Snapshots->size();
  }

  emit on_maxProgressUpdate(MaxProgress);
}

void Analyzer::CalcMaxProgressAnalyze() {
  MaxProgress -= STARTING_ESTIMATE_PROGRESS;
  MaxProgress += Results->ParticlePopulation.size() * 2;

  emit on_maxProgressUpdate(MaxProgress);
}

/*!
 * \brief Analyzer::GetEnhancedInt
 * \param snapshots
 * \param intensityVector
 */
void Analyzer::GetEnhancedInt(Images_t *snapshots,
                              std::vector<Mat> &intensityVector) {
  if (Settings->useBacklightProjection) {
    for_each(snapshots->begin(), snapshots->end(), [&](Image_t &I) {
      cv::Mat intensity;
      GetEnhancedInt(I.BackLight, intensity);
      intensityVector.push_back(intensity);
    });
  } else {
    for_each(snapshots->begin(), snapshots->end(), [&](Image_t &I) {
      cv::Mat intensity;
      GetEnhancedInt(I.FrontLight, intensity);
      intensityVector.push_back(intensity);
    });
  }
}

/*!
 * \brief Analyzer::GetEnhancedInt
 * \param img
 * \param intensity
 */
void Analyzer::GetEnhancedInt(Mat &img, Mat &intensity) {
  Vision::Conversion IntConvertor(img.clone());
  IntConvertor.Convert(Vision::Conversion::RGB, Vision::Conversion::Intensity);
  emit on_progressUpdate(currentProgress++);
  SHOW_DEBUG_IMG(IntConvertor.ProcessedImg, uchar, 255, "RGB 2 Int", false);

  if (Settings->useBlur) {
    Vision::Enhance IntBlur(IntConvertor.ProcessedImg.clone());
    IntBlur.Blur(Settings->blurKernelSize);
    emit on_progressUpdate(currentProgress++);
    uint32_t HBK = Settings->blurKernelSize / 2;
    uint32_t BK = Settings->blurKernelSize - 1;
    if (Settings->useAdaptiveContrast) {
      Vision::Enhance IntAdaptContrast(
          IntBlur.ProcessedImg(cv::Rect(HBK, HBK,
                                        IntBlur.ProcessedImg.cols - BK,
                                        IntBlur.ProcessedImg.rows - BK))
              .clone());
      IntAdaptContrast.AdaptiveContrastStretch(
          Settings->adaptContrastKernelSize,
          Settings->adaptContrastKernelFactor);
      emit on_progressUpdate(currentProgress++);
      uint32_t HAK = Settings->adaptContrastKernelSize / 2;
      uint32_t AK = Settings->adaptContrastKernelSize - 1;
      intensity = IntAdaptContrast.ProcessedImg(
          cv::Rect(HAK, HAK, IntAdaptContrast.ProcessedImg.cols - AK,
                   IntAdaptContrast.ProcessedImg.rows - AK));
    } else {
      intensity = IntBlur.ProcessedImg(
          cv::Rect(HBK, HBK, IntBlur.ProcessedImg.cols - BK,
                   IntBlur.ProcessedImg.rows - BK));
    }
  } else if (Settings->useAdaptiveContrast) {
    Vision::Enhance IntAdaptContrast(IntConvertor.ProcessedImg.clone());
    IntAdaptContrast.AdaptiveContrastStretch(
        Settings->adaptContrastKernelSize, Settings->adaptContrastKernelFactor);
    emit on_progressUpdate(currentProgress++);
    uint32_t HAK = Settings->adaptContrastKernelSize / 2;
    uint32_t AK = Settings->adaptContrastKernelSize - 1;
    intensity = IntAdaptContrast.ProcessedImg(
        cv::Rect(HAK, HAK, IntAdaptContrast.ProcessedImg.cols - AK,
                 IntAdaptContrast.ProcessedImg.rows - AK));
  } else {
    intensity = IntConvertor.ProcessedImg;
  }
  SHOW_DEBUG_IMG(intensity, uchar, 255, "Enhanced Int", false);
}

/*!
 * \brief Analyzer::GetBW
 * \param images
 * \param BWvector
 */
void Analyzer::GetBW(std::vector<cv::Mat> &images,
                     std::vector<cv::Mat> &BWvector) {
  for_each(images.begin(), images.end(), [&](cv::Mat &I) {
    cv::Mat BW;
    GetBW(I, BW);
    BWvector.push_back(BW);
  });
}

/*!
 * \brief Analyzer::GetBW
 * \param img
 * \param BW
 */
void Analyzer::GetBW(cv::Mat &img, cv::Mat &BW) {
  Vision::Segment SegBL(img.clone());
  SegBL.sigma = Settings->sigmaFactor;
  SegBL.thresholdOffset = Settings->thresholdOffsetValue;
  SegBL.ConvertToBW(Settings->typeOfObjectsSegmented);
  emit on_progressUpdate(currentProgress++);
  SHOW_DEBUG_IMG(SegBL.ProcessedImg, uchar, 255, "Segment", true);

  cv::Mat BWholes;
  if (Settings->fillHoles) {
    Vision::Segment Fillholes(SegBL.ProcessedImg);
    Fillholes.FillHoles();
    BWholes = Fillholes.ProcessedImg;
    emit on_progressUpdate(currentProgress++);
    SHOW_DEBUG_IMG(BWholes, uchar, 255, "Fillholes", true);
  } else {
    BWholes = SegBL.ProcessedImg;
  }

  cv::Mat BWborder;
  if (Settings->ignorePartialBorderParticles) {
    Vision::Segment RemoveBB(BWholes.clone());
    RemoveBB.RemoveBorderBlobs();
    BWborder = RemoveBB.ProcessedImg;
    emit on_progressUpdate(currentProgress++);
    SHOW_DEBUG_IMG(BWborder, uchar, 255, "RemoveBorderBlobs", true);
  } else {
    BWborder = BWholes;
  }

  if (Settings->morphFilterType != Vision::MorphologicalFilter::NONE) {
    Vision::MorphologicalFilter Morph(BWborder.clone());
    cv::Mat kernel = cv::Mat::zeros(Settings->filterMaskSize,
                                    Settings->filterMaskSize, CV_8UC1);
    uint32_t hMaskSize = Settings->filterMaskSize / 2;
    cv::circle(kernel, cv::Point(hMaskSize, hMaskSize), hMaskSize + 1, 1, -1);
    switch (Settings->morphFilterType) {
    case Vision::MorphologicalFilter::CLOSE:
      Morph.Close(kernel);
      break;
    case Vision::MorphologicalFilter::OPEN:
      Morph.Open(kernel);
      break;
    case Vision::MorphologicalFilter::DILATE:
      Morph.Dilation(kernel);
      break;
    case Vision::MorphologicalFilter::ERODE:
      Morph.Erosion(kernel);
      break;
    case Vision::MorphologicalFilter::NONE:
      Morph.ProcessedImg = Morph.OriginalImg;
      break;
    }
    BW = Morph.ProcessedImg;
    emit on_progressUpdate(currentProgress++);
    SHOW_DEBUG_IMG(BW, uchar, 255, "Morphological operation", true);
  } else {
    BW = BWholes;
  }
}

/*!
 * \brief Analyzer::GetParticles
 * \param BW
 * \param snapshots
 * \param partPopulation
 */
void Analyzer::GetParticles(std::vector<Mat> &BW, Images_t *snapshots,
                            Particle::ParticleVector_t &partPopulation) {
  for (uint32_t i = 0; i < snapshots->size(); i++) {
    Vision::Segment prepBW(BW[i]);
    prepBW.GetBlobList();
    emit on_progressUpdate(currentProgress++);
    GetParticlesFromBlobList(prepBW.BlobList, &(snapshots->at(i)),
                             partPopulation);
    emit on_progressUpdate(currentProgress++);
  }
}

/*!
 * \brief Analyzer::GetParticlesFromBlobList
 * \param bloblist
 * \param snapshot
 * \param edge
 * \param partPopulation
 */
void Analyzer::GetParticlesFromBlobList(
    Vision::Segment::BlobList_t &bloblist, Image_t *snapshot,
    Particle::ParticleVector_t &partPopulation) {
  for_each(bloblist.begin(), bloblist.end(), [&](Vision::Segment::Blob_t &B) {
    Particle part;
    part.ID = currentParticleID++;
    part.PixelArea = B.Area;
    if (part.GetSiDiameter() >= SMALLEST_PARTICLE_TO_ANALYZE) {
      Vision::Segment::getOrientented(B.Img, B.Centroid, B.Theta,
                                      part.Eccentricty);
      part.RGB = Vision::Segment::CopyMat<uchar>(snapshot->FrontLight(B.ROI),
                                                    B.Img, CV_8UC3)
                        .clone();
      part.BW = B.Img.clone();
      Vision::Segment edgeSeg(part.BW);
      edgeSeg.GetEdgesEroding();
      part.Edge = edgeSeg.ProcessedImg.clone();
      part.SIPixelFactor = snapshot->SIPixelFactor;
      part.isPreparedForAnalysis = false;
      part.SetRoundness();
      partPopulation.push_back(part);
      }
  });
}

/*!
 * \brief Analyzer::GetFFD
 * \param particalPopulation
 */
void Analyzer::GetFFD(Particle::ParticleVector_t &particalPopulation) {
  // for_each(particalPopulation.begin(), particalPopulation.end(),
  // [&](Particle &P) {
  QtConcurrent::blockingMap<Particle::ParticleVector_t>(
      particalPopulation, [&](Particle &P) {
        if (!P.isPreparedForAnalysis) {
          try {
            SoilMath::FFT fft;
            P.FFDescriptors = fft.GetDescriptors(P.Edge);
            P.isPreparedForAnalysis = true;
          } catch (SoilMath::Exception::MathException &e) {
            if (*e.id() == EXCEPTION_NO_CONTOUR_FOUND_NR) {
              P.isSmall = true;
            }
          }
          emit on_progressUpdate(currentProgress++);
        }
      });
}

/*!
 * \brief Analyzer::GetPrediction
 * \param particlePopulation
 */
void Analyzer::GetPrediction(Particle::ParticleVector_t &particlePopulation) {
  for_each(particlePopulation.begin(), particlePopulation.end(),
           [&](Particle &P) {
             if (P.isPreparedForAnalysis) {
               if (!P.isSmall) {
                 ComplexVect_t usedFFDescr(P.FFDescriptors.begin(),
                                           P.FFDescriptors.begin() +
                                               NeuralNet.GetInputNeurons());
                 P.Classification = NeuralNet.Predict(usedFFDescr);
                 P.isAnalysed = true;
               }
             }
           });
}

float Analyzer::CalibrateSI(float SI, Mat &img) {
  Vision::Conversion greyConv(img);
  greyConv.Convert(Vision::Conversion::RGB, Vision::Conversion::Intensity);
  Vision::Segment segment(greyConv.ProcessedImg);
  segment.ConvertToBW(Vision::Segment::Dark);
  segment.GetBlobList(true);
  uint32_t maxCircle = 0;
  for_each(segment.BlobList.begin(), segment.BlobList.end(),
           [&](Vision::Segment::Blob_t &B) {
             if (B.ROI.height > maxCircle) {
               maxCircle = B.ROI.height;
             }
             if (B.ROI.width > maxCircle) {
               maxCircle = B.ROI.width;
             }
           });
  qDebug() << "Maximum circle in pixels: " << maxCircle;
  CurrentSIfactor = SI / maxCircle;
  qDebug() << "Current SI factor : " << CurrentSIfactor;
  SIfactorDet = true;
  return CurrentSIfactor;
}

Analyzer::ExportParticles_t Analyzer::Export() {
  ExportParticles_t retVal;
  if (!Results->isAnalysed) {
    return retVal;
  }
  for_each(Results->ParticlePopulation.begin(),
           Results->ParticlePopulation.end(), [&](Particle &P) {
             ExportData_t particle;
             particle.ID = P.ID;
             particle.Area = P.PixelArea;
             particle.Radius = P.GetSiDiameter();
             particle.SIfactor = P.SIPixelFactor;
             particle.Sphericity = P.Eccentricty;
             particle.Img = P.RGB;
             retVal.push_back(particle);
           });
  return retVal;
}
}
