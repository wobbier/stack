﻿#pragma once

#include "Game.h"

#if ME_PLATFORM_UWP
#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#endif