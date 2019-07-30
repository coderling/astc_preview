cmake_minimum_required(VERSION 2.6)

project(astc_wrap)

set(ASTC_DIR ../astc-encoder-1.7)

set(BIN_DIR ${astc_wrap_SOURCE_DIR}/bin)

include_directories(${astc_wrap_SOURCE_DIR})

include_directories(${ASTC_DIR}/Source)

add_compile_definitions(WRAP_EXPORT)


add_library(astc_wrap_lib SHARED
            decode_wrap.cpp
            ${ASTC_DIR}/Source/astc_averages_and_directions.cpp
            ${ASTC_DIR}/Source/astc_block_sizes2.cpp
            ${ASTC_DIR}/Source/astc_color_quantize.cpp
            ${ASTC_DIR}/Source/astc_color_unquantize.cpp
            ${ASTC_DIR}/Source/astc_compress_symbolic.cpp 
            ${ASTC_DIR}/Source/astc_compute_variance.cpp 
            ${ASTC_DIR}/Source/astc_decompress_symbolic.cpp 
            ${ASTC_DIR}/Source/astc_encoding_choice_error.cpp 
            ${ASTC_DIR}/Source/astc_find_best_partitioning.cpp 
            ${ASTC_DIR}/Source/astc_ideal_endpoints_and_weights.cpp 
            ${ASTC_DIR}/Source/astc_image_load_store.cpp 
            ${ASTC_DIR}/Source/astc_integer_sequence.cpp 
            ${ASTC_DIR}/Source/astc_kmeans_partitioning.cpp 
            ${ASTC_DIR}/Source/astc_ktx_dds.cpp 
            ${ASTC_DIR}/Source/astc_main.cpp 
            ${ASTC_DIR}/Source/astc_partition_tables.cpp 
            ${ASTC_DIR}/Source/astc_percentile_tables.cpp 
            ${ASTC_DIR}/Source/astc_pick_best_endpoint_format.cpp 
            ${ASTC_DIR}/Source/astc_quantization.cpp 
            ${ASTC_DIR}/Source/astc_stb_tga.cpp 
            ${ASTC_DIR}/Source/astc_symbolic_physical.cpp 
            ${ASTC_DIR}/Source/astc_toplevel.cpp 
            ${ASTC_DIR}/Source/astc_weight_align.cpp 
            ${ASTC_DIR}/Source/astc_weight_quant_xfer_tables.cpp 
            ${ASTC_DIR}/Source/mathlib.cpp 
            ${ASTC_DIR}/Source/softfloat.cpp
        )

# add_executable(astc_wrap Test.cpp)
# target_link_libraries(astc_wrap astc_wrap_lib)

install(TARGETS astc_wrap_lib DESTINATION ${BIN_DIR})