/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Custom Commands Header File
**
** Purpose: Specification for the CFS HC custom commands.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $ Changed HC_Battery description to relfect Component Table changes
**	   Changed HC_Propulsion description to relect that this type of
**	   control is now enabled
**	   Changed HC_SendCtrlCmd description to reflect changes in the
**	   way this command is sent
**	   cgesterling (18.Nov.2016)
**
*/

#ifndef _hc_custom_h_
#define _hc_custom_h_

#include "cfe.h"

/*************************************************************************
** Macros for each function index
*************************************************************************/
#define HC_BATTERY_FUNC       1
#define HC_PROPULSION_FUNC    2


/**
**  \brief  Custom function
**
**  \par Description
**       This function handles the mission specific custom functions.
**       It will switch on the function index parameter and call the
**       specific custom function for that function index.
**       
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Index - The function index for the custom function.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_CustomFunction
**/
boolean HC_CustomFunction(uint8 Index);


/*************************************************************************
** Mission specific functions
*************************************************************************/

/**
**  \brief Mission specific battery function
**
**  \par Description
**       This function handles the battery component. For each heater, it 
**       calculates the average of the temperatures from the thermistors that 
**       correspond to that heater. If the temperature is too high, it calls 
**       HC_SendCtrlCmd() to send a command to turn that heater off. If the 
**       temperature is too low, it calls HC_SendCtrlCmd() to turn that heater
**       on.      
**
**  \par Assumptions, External Events, and Notes:
**       Currently, HC assumes that the number of elements in the array of 
**	 thermistors per heater is equal to the number of heaters.
**
**  \param [in]
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_BatteryCmd
**/
boolean HC_Battery(void);


/**
**  \brief Mission specific propulsion function
**
**  \par Description
**       This function handles the propulsion component. For each heater, it 
**       calculates the average of the temperatures from the thermistors that 
**       correspond to that heater. If the temperature is too high, it calls 
**       HC_SendCtrlCmd() to send a command to turn that heater off. If the 
**       temperature is too low, it calls HC_SendCtrlCmd() to turn that heater
**       on. 
**
**  \par Assumptions, External Events, and Notes:
**       Currently, HC assumes that the number of elements in the array of 
**	 thermistors per heater is equal to the number of heaters.
**
**  \param [in]
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_PropulsionCmd
**/
boolean HC_Propulsion(void);


/**
**  \brief Send Heater Control Command
**
**  \par Description
**       This function sends a heater control command. It sends
**       the index of the component and heater to be controlled and a
**       flag that determines if the heater should be turned on or off.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  ComponentIndex - The index of the component to be
**       controlled.
**
**  \param [in]  HeaterIndex - The index of the heater to be
**       controlled.
**
**  \param [in]  TurnOnFlag - A flag to indicate if the heater
**       should be turned on or off.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #HC_SEND_CTRL_CMD_CC, #HC_SendCtrlPkt, #HC_SendCtrlCmd_t
**/
boolean HC_SendCtrlCmd(uint8 ComponentIndex, uint8 HeaterIndex, boolean TurnOnFlag);


#endif /* _hc_custom_h_ */

/************************/
/*  End of File Comment */
/************************/

