#pragma once

#include "PCH.h"

class Shader {
public:
	Shader();
private:
	wil::com_ptr<ID3DBlob> blob;
};