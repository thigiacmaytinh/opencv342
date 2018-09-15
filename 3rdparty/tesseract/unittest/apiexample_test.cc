///////////////////////////////////////////////////////////////////////
// File:        apiexample_test.cc
// Description: Api Test for Tesseract using text fixtures and parameters.
// Author:      ShreeDevi Kumar
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
///////////////////////////////////////////////////////////////////////

// expects clone of tessdata_fast repo in ../../tessdata_fast

//#include "log.h"
#include "include_gunit.h"
#include "baseapi.h"
#include "leptonica/allheaders.h"
#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <limits.h>
#include <time.h>

namespace {

class QuickTest : public testing::Test {
 protected:
  virtual void SetUp() {
    start_time_ = time(nullptr);
  }
  virtual void TearDown() {
    const time_t end_time = time(nullptr);
    EXPECT_TRUE(end_time - start_time_ <=25) << "The test took too long - " << ::testing::PrintToString(end_time - start_time_);
  }
  time_t start_time_;
  };

  void OCRTester(const char* imgname, const char* groundtruth, const char* tessdatadir, const char* lang) {
    //log.info() << tessdatadir << " for language: " << lang << std::endl;
    char *outText;
    std::locale loc("C"); // You can also use "" for the default system locale
    std::ifstream file(groundtruth);
    file.imbue(loc); // Use it for file input
    std::string gtText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    ASSERT_FALSE(api->Init(tessdatadir, lang)) << "Could not initialize tesseract.";
    Pix *image = pixRead(imgname);
    ASSERT_TRUE(image != nullptr) << "Failed to read test image.";
    api->SetImage(image);
    outText = api->GetUTF8Text();
    EXPECT_EQ(gtText,outText) << "Phototest.tif OCR does not match ground truth for " << ::testing::PrintToString(lang);
    api->End();
    delete [] outText;
    pixDestroy(&image);
  }

  class MatchGroundTruth : public QuickTest ,
      public ::testing::WithParamInterface<const char*> {
  };
  
  TEST_P(MatchGroundTruth, FastPhototestOCR) {
    OCRTester(TESTING_DIR "/phototest.tif",
              TESTING_DIR "/phototest.txt",
              TESSDATA_DIR "_fast", GetParam());
  }

  TEST_P(MatchGroundTruth, BestPhototestOCR) {
    OCRTester(TESTING_DIR "/phototest.tif",
              TESTING_DIR "/phototest.txt",
              TESSDATA_DIR "_best", GetParam());
  }
 
  TEST_P(MatchGroundTruth, TessPhototestOCR) {
    OCRTester(TESTING_DIR "/phototest.tif",
              TESTING_DIR "/phototest.txt",
              TESSDATA_DIR , GetParam());
  }
 
  INSTANTIATE_TEST_CASE_P( Eng, MatchGroundTruth, 
                        ::testing::Values("eng") );
  INSTANTIATE_TEST_CASE_P( Latin, MatchGroundTruth, 
                        ::testing::Values("script/Latin") );
  INSTANTIATE_TEST_CASE_P( Deva, MatchGroundTruth, 
                        ::testing::Values("script/Devanagari") );
  INSTANTIATE_TEST_CASE_P( Arab, MatchGroundTruth, 
                        ::testing::Values("script/Arabic") );
  
  class EuroText : public QuickTest {
  };
  
  TEST_F(EuroText, FastLatinOCR) {
    OCRTester(TESTING_DIR "/eurotext.tif",
              TESTING_DIR "/eurotext.txt",
              TESSDATA_DIR "_fast", "script/Latin");
  }

  // script/Latin for eurotext.tif does not match groundtruth 
  // for tessdata & tessdata_best
  // so do not test these here.
  
}  // namespace
