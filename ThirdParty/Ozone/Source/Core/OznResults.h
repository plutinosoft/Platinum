/*****************************************************************
|
|   Ozone - Result Codes
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef _OZN_RESULTS_H_
#define _OZN_RESULTS_H_

/*----------------------------------------------------------------------
|   macros
+---------------------------------------------------------------------*/
#define OZN_SUCCESS   0
#define OZN_FAILURE (-1)

#define OZN_FAILED(result)             ((result) != OZN_SUCCESS)
#define OZN_SUCCEEDED(result)          ((result) == OZN_SUCCESS)

/*----------------------------------------------------------------------
|    result codes      
+---------------------------------------------------------------------*/

/* general error codes */
#define OZN_ERROR_BASE                  (-30000)

#define OZN_ERROR_BUSY                  (OZN_ERROR_BASE - 0)
#define OZN_ERROR_INVALID_PROPERTY      (OZN_ERROR_BASE - 1)
#define OZN_ERROR_MISSING_PROPERTY      (OZN_ERROR_BASE - 2)
#define OZN_ERROR_NO_MORE_ITEMS         (OZN_ERROR_BASE - 3)
#define OZN_ERROR_DB_INVALID_FILEPATH   (OZN_ERROR_BASE - 4)

#endif /* _OZN_RESULTS_H_ */