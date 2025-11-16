/*
* Multi-Client Server File Transfer
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

/**
 * @file ProcessFileTransfer.h
 *
 * @brief Processor for handling file transfer from server to client
 *
 * @author Adama Zouma
 * 
 * @Contact: stargue49@gmail.com
 *
 */

#ifndef PROCESS_FILE_TRANSFER_H
#define PROCESS_FILE_TRANSFER_H
#include "MyProcessor.h"

namespace za{
/**
 * Implementation of a derived class used to transfer files to client
 * The parent class is MyProcessor, an abstract class
 */
class ProcessFileTransfer:public za::MyProcessor
{
public:
	/* ============================================================================
	* Data Memeber Declaration
	* ============================================================================
	*/
	//none


	/* ============================================================================
	* Member Function Declaration
	* ============================================================================
	*/
	void process(za::MyServer _serverProcessingAcceptedConnexion_);

};
}
#endif /*PROCESS_FILE_TRANSFER_H */
