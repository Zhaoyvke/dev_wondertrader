
CXX=g++
OUT_NAME=sl_efh_lev2_recv_demo

OUT_PATH=./bin/
SRC_PATH=./
OBJ_PATH=./obj/

DEBUG_PATH=debug/
RELEASE_PATH=release/

DEBUG_DL_PATH=-ldl -lrt

RELEASE_DL_PATH=-ldl -lrt


SRC_FILE=$(wildcard $(SRC_PATH) *.cpp)
ALL_FILE=$(notdir $(SRC_FILE))
OBJ_FILE=$(patsubst %.cpp,%.o,$(ALL_FILE))

DEBUG_CXXFLAGS=-g -ggdb3 -Wall -fPIC
RELEASE_CXXFLAGS=-O2 -fPIC

DEBUG_ARFLAGS=cqs
RELEASE_ARFLAGS=cqs

OUT_DEBUG_FILE=$(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME)
OBJ_DEBUG_FILE=$(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(notdir $(OBJ_FILE)))

OUT_RELEASE_FILE=$(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME)
OBJ_RELEASE_FILE=$(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(notdir $(OBJ_FILE)))

all : debug release

debug : pre_debug $(OUT_DEBUG_FILE)

release : per_release $(OUT_RELEASE_FILE)

pre_debug :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(DEBUG_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell mkdir $(OUT_PATH)$(DEBUG_PATH) -p)
	-$(shell cp ../efh_lev2_api/linux/debug/libsl_efh_lev2_api.so $(OUT_PATH)$(DEBUG_PATH) -rf)
	-$(shell cp ./efh_recv_conf.ini $(OUT_PATH)$(DEBUG_PATH) -rf)

per_release :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(RELEASE_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell mkdir $(OUT_PATH)$(RELEASE_PATH) -p)
	-$(shell cp ../efh_lev2_api/linux/release/libsl_efh_lev2_api.so $(OUT_PATH)$(RELEASE_PATH) -rf)
	-$(shell cp ./efh_recv_conf.ini $(OUT_PATH)$(RELEASE_PATH) -rf)

$(OUT_DEBUG_FILE) : $(OBJ_DEBUG_FILE)
	$(CXX) $(DEBUG_CXXFLAGS) $(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(notdir $^)) -o $@ $(DEBUG_DL_PATH)

$(OUT_RELEASE_FILE) : $(OBJ_RELEASE_FILE)
	$(CXX) $(RELEASE_CXXFLAGS) $(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(notdir $^)) -o $@ $(RELEASE_DL_PATH)

$(OBJ_PATH)$(DEBUG_PATH)%.o : %.cpp
	$(CXX) -c $(DEBUG_CXXFLAGS) $(SRC_PATH)$< -o $@

$(OBJ_PATH)$(RELEASE_PATH)%.o : %.cpp
	$(CXX) -c $(RELEASE_CXXFLAGS) $(SRC_PATH)$< -o $@

clean :
	@echo  "make clean"
	$(shell rm $(OBJ_PATH) -rf)
	$(shell rm $(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME) -f)
	$(shell rm $(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME) -f)
