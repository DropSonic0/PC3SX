/*
 * IGraphics.h
 *
 *  Created on: Oct 26, 2010
 *      Author: halsafar
 */

#ifndef __IGRAPHICS_H__
#define __IGRAPHICS_H__

template <class FLOAT_TYPE=float, class INT_TYPE=int>
class IGraphics
{
public:
	virtual ~IGraphics() {}

	virtual void Init() = 0;
	virtual void Deinit() = 0;

	virtual void InitDbgFont() = 0;
	virtual void DeinitDbgFont() = 0;

	virtual FLOAT_TYPE GetDeviceAspectRatio() = 0;
	virtual INT_TYPE GetResolutionWidth() = 0;
	virtual INT_TYPE GetResolutionHeight() = 0;
private:
};

#endif
