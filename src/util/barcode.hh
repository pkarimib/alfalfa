#pragma once

#include <iostream>
#include "raster_handle.hh"

namespace pantea_cc {

uint64_t inline read_barcode(const RasterHandle raster_handle) {
  constexpr int64_t BARCODE_WIDTH = 128;
  constexpr int64_t SQUARE_WIDTH = BARCODE_WIDTH / 8;
  auto & raster = raster_handle.get();
  uint64_t barcode_value = 0;

  for (int64_t i = 0; i < 8; i++) {
    for (int64_t j = 0; j < 8; j++) {
      // for each square in the barcode
      int64_t avg_y, avg_u, avg_v;
      avg_y = avg_u = avg_v = 0;

      for (int64_t x = i * SQUARE_WIDTH; x < (i + 1) * SQUARE_WIDTH; x++) {
        for (int64_t y = j * SQUARE_WIDTH; y < (j + 1) * SQUARE_WIDTH; y++) {
          // avg_y += buf_i420->DataY()[x * buf_i420->StrideY() + y];
          // avg_u += buf_i420->DataU()[(x / 2) * buf_i420->StrideU() + y / 2];
          // avg_v += buf_i420->DataV()[(x / 2) * buf_i420->StrideV() + y / 2];
          avg_y += raster.Y().at(x, y);
          avg_u += raster.U().at(x / 2, y / 2);
          avg_v += raster.V().at(x / 2, y / 2);
        }
      }

      avg_y /= (SQUARE_WIDTH * SQUARE_WIDTH);
      avg_u /= (SQUARE_WIDTH * SQUARE_WIDTH);
      avg_v /= (SQUARE_WIDTH * SQUARE_WIDTH);

      const auto white_err = (avg_y - 255) * (avg_y - 255) +
                             (avg_u - 128) * (avg_u - 128) +
                             (avg_v - 128) * (avg_v - 128);

      const auto black_err = (avg_y - 0) * (avg_y - 0) +
                             (avg_u - 128) * (avg_u - 128) +
                             (avg_v - 128) * (avg_v - 128);

      if (black_err < white_err) {
        barcode_value |= (1ull << (i * 8 + j));
      }
    }
  }

  return barcode_value;
}

}  // namespace pantea_cc
