#include "plugin.h"
#include "CCamera.h"
#include "IniReader/IniReader.h"

using namespace plugin;

bool aimingLastFrame = false;
float m_fMouseAccelVerticalOriginal, m_fMouseAccelHorzntlOriginal, sensibMult, sniperSensibMult;

class AimSensibility {
public:
    AimSensibility() {
		CIniReader ini("AimSensibility.ini");
		sensibMult = ini.ReadFloat("Settings", "SensibMult", 0.5f);
		sniperSensibMult = ini.ReadFloat("Settings", "SniperSensibMult", 0.5f);

		Events::processScriptsEvent.before +=[] {
			eCamMode mode = (eCamMode)TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
			if (mode == eCamMode::MODE_AIMING || mode == eCamMode::MODE_SNIPER || mode == eCamMode::MODE_ROCKETLAUNCHER ||
				mode == eCamMode::MODE_CAMERA || mode == eCamMode::MODE_ROCKETLAUNCHER_HS || mode == eCamMode::MODE_AIMWEAPON || mode == eCamMode::MODE_AIMWEAPON_ATTACHED) {
				if (!aimingLastFrame) {
					m_fMouseAccelVerticalOriginal = TheCamera.m_fMouseAccelVertical;
					m_fMouseAccelHorzntlOriginal = TheCamera.m_fMouseAccelHorzntl;
					float sensib = (mode == eCamMode::MODE_SNIPER) ? sniperSensibMult : sensibMult;
					TheCamera.m_fMouseAccelVertical = m_fMouseAccelVerticalOriginal * sensib;
					TheCamera.m_fMouseAccelHorzntl = m_fMouseAccelHorzntlOriginal * sensib;
					aimingLastFrame = true;
				}
			} else aimingLastFrame = false;
		};
		Events::drawingEvent.after += [] { ResetSensibility(); }; // resets as soon as possible, to avoid being saved when exiting the game
    }

	static void ResetSensibility() {
		if (aimingLastFrame) {
			TheCamera.m_fMouseAccelVertical = m_fMouseAccelVerticalOriginal;
			TheCamera.m_fMouseAccelHorzntl = m_fMouseAccelHorzntlOriginal;
			aimingLastFrame = false;
		}
	}
} aimSensibility;
