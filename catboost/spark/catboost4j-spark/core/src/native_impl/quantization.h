#pragma once

#include <catboost/libs/data/objects.h>
#include <catboost/libs/data/quantized_features_info.h>

#include <util/generic/fwd.h>
#include <util/generic/array_ref.h>
#include <util/generic/vector.h>
#include <util/generic/yexception.h>
#include <util/system/types.h>


namespace NCB {
    class TFeaturesLayout;
}


NCB::TQuantizedFeaturesInfoPtr PrepareQuantizationParameters(
    const NCB::TFeaturesLayout& featuresLayout,
    const TString& plainJsonParamsAsString
) throw (yexception);


class TNanModeAndBordersBuilder {
public:
    TNanModeAndBordersBuilder(NCB::TQuantizedFeaturesInfoPtr quantizedFeaturesInfo) throw (yexception);

    bool HasFeaturesToCalc() const {
        return !FeatureIndicesToCalc.empty();
    }

    // call before Finish and preferably before adding samples
    void SetSampleSize(i32 sampleSize) throw (yexception);

    void AddSample(TConstArrayRef<double> objectData) throw (yexception);

    // updates parameters in quantizedFeaturesInfo passed to constructor
    void Finish(i32 threadCount) throw (yexception);

private:
    size_t SampleSize = 0;
    TVector<ui32> FeatureIndicesToCalc;
    NCB::TQuantizedFeaturesInfoPtr QuantizedFeaturesInfo;
    TVector<TVector<float>> Data; // [featureIdxToCalc]
};


NCB::TQuantizedObjectsDataProviderPtr Quantize(
    NCB::TQuantizedFeaturesInfoPtr quantizedFeaturesInfo,
    NCB::TRawObjectsDataProviderPtr* rawObjectsDataProvider, // moved into
    int threadCount
) throw (yexception);


void GetActiveFeaturesIndices(
    NCB::TFeaturesLayoutPtr featuresLayout,
    NCB::TQuantizedFeaturesInfoPtr quantizedFeaturesInfo,
    TVector<i32>* ui8FlatIndices,
    TVector<i32>* ui16FlatIndices,
    TVector<i32>* ui32FlatIndices
) throw (yexception);
