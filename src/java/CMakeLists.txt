# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

########### next target ###############

set(
  ical_jni_LIB_SRCS
  jlibical_consts_cxx.h
  jlibical_utils_cxx.cpp
  jlibical_utils_cxx.h
  jniICalDurationType_cxx.cpp
  jniICalDurationType_cxx.h
  jniICalPeriodType_cxx.cpp
  jniICalPeriodType_cxx.h
  jniICalRecurrenceType_cxx.cpp
  jniICalRecurrenceType_cxx.h
  jniICalTimeType_cxx.cpp
  jniICalTimeType_cxx.h
  jniICalTriggerType_cxx.cpp
  jniICalTriggerType_cxx.h
  net_cp_jlibical_ICalParameter_cxx.cpp
  net_cp_jlibical_ICalParameter_cxx.h
  net_cp_jlibical_ICalProperty_cxx.cpp
  net_cp_jlibical_ICalProperty_cxx.h
  net_cp_jlibical_ICalValue_cxx.cpp
  net_cp_jlibical_ICalValue_cxx.h
  net_cp_jlibical_VComponent_cxx.cpp
  net_cp_jlibical_VComponent_cxx.h
)

add_library(
  ical_jni
  ${LIBRARY_TYPE}
  ${ical_jni_LIB_SRCS}
)

target_link_libraries(ical_jni ical_cxx)
if(NOT ANDROID)
  set_target_properties(
    ical_jni
    PROPERTIES
      VERSION
        ${CMAKE_PROJECT_VERSION}
      SOVERSION
        ${LIBICAL_LIB_SOVERSION_STRING}
  )
endif()
install(TARGETS ical_jni EXPORT icalTargets DESTINATION lib)

########### install files ###############

install(FILES DESTINATION /include)
install(FILES libical.jar DESTINATION /share)
