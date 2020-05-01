/*
** Purpose: Define cFE Table 
**
** Notes:
**   1. Can't register a table during construction because the app has not been
**      registered with ES yet.
**   2. This is proto that doesn't expose all of the cFE table services.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _oskfw_cfe_tbl_   // _cfe_tbl_ is used by the cFE
#define _oskfw_cfe_tbl_

/*
** Includes
*/

#include <bits/stdc++.h> 
extern "C" {
   #include "cfe.h"
}


namespace oskfw {


/******************************************************************************
** Table Class
*/
template <typename T> class CfeTbl {
public:
   
   CfeTbl(const std::string& name, uint16 options);
   virtual ~CfeTbl() {}
   
   // Register a table with no default data, data from a buffer or data from a file
   bool Register(CFE_TBL_CallbackFuncPtr_t callback_func);
   bool Register(CFE_TBL_CallbackFuncPtr_t callback_func, const T* data_buffer);
   bool Register(CFE_TBL_CallbackFuncPtr_t callback_func, char* data_filename);

   void PollCfeTblService();
 
   T* GetAddress();
   void ReleaseAddress();

   bool   address_in_use() { return m_address_in_use; }
   int32  status()         { return m_status;         }
   
protected:

   std::string  m_name;
   uint16       m_options;
   
   CFE_TBL_Handle_t           m_handle;
   CFE_TBL_CallbackFuncPtr_t  m_callback_func;
   
   int32  m_status;           // Status of last cFE Tbl service call
   bool   m_address_in_use;

   // Tables shouldn't need to be copied
   CfeTbl(const CfeTbl &tbl) {}  
   CfeTbl& operator=(const CfeTbl &tbl) {} 

}; // End class CfeTbl

/******************************************************************************
** Constructors/Destructors 
**
*/

template <typename T>
CfeTbl<T>::CfeTbl(const std::string& name, uint16 options) {

   m_name    = name;
   m_options = options;

   m_address_in_use = FALSE;
      
} // End CfeTbl()

/******************************************************************************
** Periodic Processing
**
*/

template <typename T>
void CfeTbl<T>::PollCfeTblService() {

   CFE_TBL_Manage(m_handle); 

}

/******************************************************************************
** Register/Load Table 
**
*/

template <typename T>
bool CfeTbl<T>::Register(CFE_TBL_CallbackFuncPtr_t callback_func) {

   m_address_in_use = FALSE;
   
   m_status = CFE_TBL_Register(&m_handle, m_name.c_str(), sizeof(T), m_options, callback_func);
    
   if (m_status != CFE_SUCCESS) {
   
      CFE_ES_WriteToSysLog("Error registering table %s, CFE_TBL_Register() status = 0x%08X\n", m_name.c_str(), m_status);
   
   }

   return (m_status == CFE_SUCCESS);

} // End Register()


template <typename T>
bool CfeTbl<T>::Register(CFE_TBL_CallbackFuncPtr_t callback_func, const T* data_buffer) {
   
   if (Register(callback_func)) {
   
      m_status = CFE_TBL_Load(m_handle, CFE_TBL_SRC_ADDRESS, (void *)data_buffer);

   }

} // End Register()


template <typename T>
bool CfeTbl<T>::Register(CFE_TBL_CallbackFuncPtr_t callback_func, char* data_filename) {

   if (Register(callback_func)) {
   
      m_status = CFE_TBL_Load(m_handle, CFE_TBL_SRC_FILE, data_filename);

   }
   
} // End Register()


/******************************************************************************
** Get/Release Address
*/

template<typename T>
T* CfeTbl<T>::GetAddress() {
   
   T* data_ptr = nullptr;
   
   m_status = CFE_TBL_GetAddress((void **)&data_ptr, m_handle);

   m_address_in_use = (m_status == CFE_SUCCESS);
   
   return data_ptr;
   
} // End GetAddress()


template<typename T>
void CfeTbl<T>::ReleaseAddress() {
   
   if (m_address_in_use) {
      
      m_status = CFE_TBL_ReleaseAddress(m_handle);
   
   }

   // Always clear to avoid odd latchup scenario
   m_address_in_use = FALSE; 
   
} // End ReleaseAddress()


} // End namespace oskfw

#endif // _oskfw_cfe_tbl_ 
