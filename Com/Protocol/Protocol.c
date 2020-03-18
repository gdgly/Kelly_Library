bool Protocol_CheckWithTimeOut(void) //blocking
{
	char charBuffer;
	uint32_t startTime = Millis();
	uint32_t timeOutCounter = 0;

	while (Millis() - startTime < FC_START_UP_TIME_OUT + 1)
	{
		Serial_SendChar(FC_CMD_ACK);
		timeOutCounter += FC_START_UP_TIME_OUT_PER_ACK;

		while (Millis() - startTime < timeOutCounter) // wait 250ms for Rx Char
		{
			if (Serial_GetCharsInRxBuf())
			{

				Serial_RecvChar(&charBuffer);
					fp(charBuffer);
			}
		}
	}
	return false;
}
