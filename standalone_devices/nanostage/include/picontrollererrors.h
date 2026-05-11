// PIControllerErrors.h
//
// This file defines symbols for each error code used in C/C++ programs.
//
// This file is automagically generated from the central error code list.
// DO NOT ADD ERROR CODES IN THIS FILE! Use the error list and the generation tool instead!

#ifndef __PI_CONTROLLER_ERROS_H__
#define __PI_CONTROLLER_ERROS_H__

//////////////////////////////////////////////////
//
// Dll Errors - DLL errors occured in GCS DLL
//
#define PI_UNKNOWN_AXIS_IDENTIFIER  -1001L  /**< \ingroup err
		Unknown axis identifier */
#define PI_NR_NAV_OUT_OF_RANGE  -1002L  /**< \ingroup err
		Number for NAV out of range--must be in [1,10000] */
#define PI_INVALID_SGA  -1003L  /**< \ingroup err
		Invalid value for SGA--must be one of {1, 10, 100, 1000} */
#define PI_UNEXPECTED_RESPONSE  -1004L  /**< \ingroup err
		Controller sent unexpected response */
#define PI_NO_MANUAL_PAD  -1005L  /**< \ingroup err
		No manual control pad installed, calls to SMA and related commands are not allowed */
#define PI_INVALID_MANUAL_PAD_KNOB  -1006L  /**< \ingroup err
		Invalid number for manual control pad knob */
#define PI_INVALID_MANUAL_PAD_AXIS  -1007L  /**< \ingroup err
		Axis not currently controlled by a manual control pad */
#define PI_CONTROLLER_BUSY  -1008L  /**< \ingroup err
		Controller is busy with some lengthy operation (e.g. reference move, fast scan algorithm) */
#define PI_THREAD_ERROR  -1009L  /**< \ingroup err
		Internal error--could not start thread */
#define PI_IN_MACRO_MODE  -1010L  /**< \ingroup err
		Controller is (already) in macro mode--command not valid in macro mode */
#define PI_NOT_IN_MACRO_MODE  -1011L  /**< \ingroup err
		Controller not in macro mode--command not valid unless macro mode active */
#define PI_MACRO_FILE_ERROR  -1012L  /**< \ingroup err
		Could not open file to write or read macro */
#define PI_NO_MACRO_OR_EMPTY  -1013L  /**< \ingroup err
		No macro with given name on controller, or macro is empty */
#define PI_MACRO_EDITOR_ERROR  -1014L  /**< \ingroup err
		Internal error in macro editor */
#define PI_INVALID_ARGUMENT  -1015L  /**< \ingroup err
		One or more arguments given to function is invalid (empty string, index out of range, ...) */
#define PI_AXIS_ALREADY_EXISTS  -1016L  /**< \ingroup err
		Axis identifier is already in use by a connected stage */
#define PI_INVALID_AXIS_IDENTIFIER  -1017L  /**< \ingroup err
		Invalid axis identifier */
#define PI_COM_ARRAY_ERROR  -1018L  /**< \ingroup err
		Could not access array data in COM server */
#define PI_COM_ARRAY_RANGE_ERROR  -1019L  /**< \ingroup err
		Range of array does not fit the number of parameters */
#define PI_INVALID_SPA_CMD_ID  -1020L  /**< \ingroup err
		Invalid parameter ID given to SPA or SPA? */
#define PI_NR_AVG_OUT_OF_RANGE  -1021L  /**< \ingroup err
		Number for AVG out of range--must be >0 */
#define PI_WAV_SAMPLES_OUT_OF_RANGE  -1022L  /**< \ingroup err
		Incorrect number of samples given to WAV */
#define PI_WAV_FAILED  -1023L  /**< \ingroup err
		Generation of wave failed */
#define PI_MOTION_ERROR  -1024L  /**< \ingroup err
		Motion error while axis in motion, call CLR to resume operation */
#define PI_RUNNING_MACRO  -1025L  /**< \ingroup err
		Controller is (already) running a macro */
#define PI_PZT_CONFIG_FAILED  -1026L  /**< \ingroup err
		Configuration of PZT stage or amplifier failed */
#define PI_PZT_CONFIG_INVALID_PARAMS  -1027L  /**< \ingroup err
		Current settings are not valid for desired configuration */
#define PI_UNKNOWN_CHANNEL_IDENTIFIER  -1028L  /**< \ingroup err
		Unknown channel identifier */
#define PI_WAVE_PARAM_FILE_ERROR  -1029L  /**< \ingroup err
		Error while reading/writing wave generator parameter file */
#define PI_UNKNOWN_WAVE_SET  -1030L  /**< \ingroup err
		Could not find description of wave form. Maybe WG.INI is missing? */
#define PI_WAVE_EDITOR_FUNC_NOT_LOADED  -1031L  /**< \ingroup err
		The WGWaveEditor DLL function was not found at startup */
#define PI_USER_CANCELLED  -1032L  /**< \ingroup err
		The user cancelled a dialog */
#define PI_C844_ERROR  -1033L  /**< \ingroup err
		Error from C-844 Controller */
#define PI_DLL_NOT_LOADED  -1034L  /**< \ingroup err
		DLL necessary to call function not loaded, or function not found in DLL */
#define PI_PARAMETER_FILE_PROTECTED  -1035L  /**< \ingroup err
		The open parameter file is protected and cannot be edited */
#define PI_NO_PARAMETER_FILE_OPENED  -1036L  /**< \ingroup err
		There is no parameter file open */
#define PI_STAGE_DOES_NOT_EXIST  -1037L  /**< \ingroup err
		Selected stage does not exist */
#define PI_PARAMETER_FILE_ALREADY_OPENED  -1038L  /**< \ingroup err
		There is already a parameter file open. Close it before opening a new file */
#define PI_PARAMETER_FILE_OPEN_ERROR  -1039L  /**< \ingroup err
		Could not open parameter file */
#define PI_INVALID_CONTROLLER_VERSION  -1040L  /**< \ingroup err
		The version of the connected controller is invalid */
#define PI_PARAM_SET_ERROR  -1041L  /**< \ingroup err
		Parameter could not be set with SPA--parameter not defined for this controller! */
#define PI_NUMBER_OF_POSSIBLE_WAVES_EXCEEDED  -1042L  /**< \ingroup err
		The maximum number of wave definitions has been exceeded */
#define PI_NUMBER_OF_POSSIBLE_GENERATORS_EXCEEDED  -1043L  /**< \ingroup err
		The maximum number of wave generators has been exceeded */

#endif // __PI_CONTROLLER_ERROS_H__
