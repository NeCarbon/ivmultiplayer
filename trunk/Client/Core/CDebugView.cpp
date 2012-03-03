//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: CDebugView.cpp
// Project: Client.Core
// Author(s): jenksta
// License: See LICENSE in root directory
//
//==============================================================================

#include "CDebugView.h"
#include "CChatWindow.h"
#include "CGUI.h"
#include "CLocalPlayer.h"

#define DEBUG_TEXT_TOP (40.0f + (MAX_DISPLAYED_MESSAGES * 20))

extern CGUI         * g_pGUI;
extern CLocalPlayer * g_pLocalPlayer;

CDebugView::CDebugView()
{
	m_fDebugTextTop = 0;
}

CDebugView::~CDebugView()
{

}

void CDebugView::DrawText(String strText)
{
	// Get the font
	CEGUI::Font * pFont = NULL/*g_pGUI->GetFont("tahoma-bold", 10)*/;

	// Draw the text
	g_pGUI->DrawText(strText, CEGUI::Vector2(26.0f, m_fDebugTextTop), CEGUI::colour(0xFFFFFFFF), pFont);

	// Increment the text top
	m_fDebugTextTop += 14.0f;
}

void CDebugView::DumpTask(String strName, CIVTask * pTask)
{
	if(!pTask)
	{
		//DrawDebugText(String("%s: None (9999)", strName.Get()));
		return;
	}

	DrawText(String("%s: %s (%d)", strName.Get(), pTask->GetName(), pTask->GetType()));

	/*if(!pTask->IsSimple())
	{
		CIVTask * pSubTask = ((CIVTaskComplex *)pTask)->GetSubTask();
		DumpTask(strName, pSubTask);
	}*/
}

void CDebugView::DumpTasks(CIVPedTaskManager * pPedTaskManager, int iType)
{
	if(iType == 0)
	{
		DrawText("Priority Tasks: ");
		DrawText("");
		DumpTask("PhysicalResponse",     pPedTaskManager->GetTask(TASK_PRIORITY_PHYSICAL_RESPONSE));
		DumpTask("EventResponseTemp",    pPedTaskManager->GetTask(TASK_PRIORITY_EVENT_RESPONSE_TEMP));
		DumpTask("EventResponseNonTemp", pPedTaskManager->GetTask(TASK_PRIORITY_EVENT_RESPONSE_NONTEMP));
		DumpTask("Primary",              pPedTaskManager->GetTask(TASK_PRIORITY_PRIMARY));
		DumpTask("Default",              pPedTaskManager->GetTask(TASK_PRIORITY_DEFAULT));
		DrawText("");
	}
	else if(iType == 1)
	{
		DrawText("Secondary Tasks: ");
		DrawText("");
		DumpTask("Attack",        pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
		DumpTask("Duck",          pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_DUCK));
		DumpTask("Say",           pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_SAY));
		DumpTask("FacialComplex", pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
		DumpTask("PartialAnim",   pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM));
		DumpTask("IK",            pPedTaskManager->GetTaskSecondary(TASK_SECONDARY_IK));
		DrawText("");

	}
	else if(iType == 2)
	{
		DrawText("Movement Tasks: ");
		DrawText("");
		DumpTask("MovementTask0", pPedTaskManager->GetTaskMovement(TASK_MOVEMENT_UNKNOWN0));
		DumpTask("MovementTask1", pPedTaskManager->GetTaskMovement(TASK_MOVEMENT_UNKNOWN1));
		DumpTask("MovementTask2", pPedTaskManager->GetTaskMovement(TASK_MOVEMENT_UNKNOWN2));
		DrawText("");
	}
}

void CDebugView::DumpPlayer(CNetworkPlayer * pPlayer)
{
	DrawText("Player Debug: ");
	DrawText("");

	// Position data
	CVector3 vecPosition;
	pPlayer->GetPosition(vecPosition);
	float fCurrentHeading = pPlayer->GetCurrentHeading();
	float fDesiredHeading = pPlayer->GetDesiredHeading();
	DrawText(String("Position: %f, %f, %f Heading (C/D): %f, %f", vecPosition.fX, vecPosition.fY, vecPosition.fZ, fCurrentHeading, fDesiredHeading));

	// Speed data
	CVector3 vecMoveSpeed;
	pPlayer->GetMoveSpeed(vecMoveSpeed);
	DrawText(String("Move Speed: %f, %f, %f", vecMoveSpeed.fX, vecMoveSpeed.fY, vecMoveSpeed.fZ));

	// Camera data
	// TODO: Make player specific
#if 0
	CIVCam * pGameCam = g_pCamera->GetGameCam();
	CVector3 vecCamPosition;
	pGameCam->GetPosition(vecCamPosition);
	CVector3 vecCamForward;
	vecCamForward = pGameCam->GetCam()->m_data1.m_matMatrix.vecForward;
	CVector3 vecLookAt;
	vecLookAt.fX = vecCamPosition.fX + /*(*/vecCamForward.fX/* * fScale)*/;
	vecLookAt.fY = vecCamPosition.fY + /*(*/vecCamForward.fY/* * fScale)*/;
	vecLookAt.fZ = vecCamPosition.fZ + /*(*/vecCamForward.fZ/* * fScale)*/;
	DrawText(String("Camera Position: %f, %f, %f Camera Look At: %f, %f, %f", vecCamPosition.fX, vecCamPosition.fY, vecCamPosition.fZ, vecLookAt.fX, vecLookAt.fY, vecLookAt.fZ));
#endif

	// Health, armour and ducking data
	unsigned int uiHealth = pPlayer->GetHealth();
	unsigned int uiArmour = pPlayer->GetArmour();
	bool bDucking = pPlayer->IsDucking();
	DrawText(String("Health: %d Armour: %d Ducking: %d", uiHealth, uiArmour, bDucking));

	// Weapon data
	unsigned int uiWeaponId = pPlayer->GetCurrentWeapon();
	unsigned int uiAmmo = pPlayer->GetAmmo(uiWeaponId);
	DrawText(String("Weapon: %d Ammo: %d", uiWeaponId, uiAmmo));

	// Damage data
	IVEntity * pDamageEntity = pPlayer->GetGamePlayerPed()->GetPhysical()->m_pLastDamageEntity;
	eWeaponType damageWeapon = pPlayer->GetGamePlayerPed()->GetPhysical()->m_lastDamageWeapon;
	DrawText(String("Last Damage Entity: 0x%x Last Damage Weapon: %d", pDamageEntity, (unsigned int)damageWeapon));

	// Vehicle entry/exit
	String strEntryExit = "None";

	if(pPlayer->IsGettingInToAVehicle())
		strEntryExit = "Getting In";
	else if(pPlayer->IsGettingOutOfAVehicle())
		strEntryExit = "Getting Out";

	DrawText(String("Vehicle Entry/Exit: %s", strEntryExit.Get()));

	DrawText("");

	// Tasks
	CIVPedTaskManager * pPedTaskManager = pPlayer->GetGamePlayerPed()->GetPedTaskManager();

	// Priority task data
	DumpTasks(pPedTaskManager, 0);

	// Secondary task data
	DumpTasks(pPedTaskManager, 1);

	// Movement task data
	DumpTasks(pPedTaskManager, 2);
}

void CDebugView::Draw()
{
	if(g_pGUI)
	{
		m_fDebugTextTop = DEBUG_TEXT_TOP;

		// TODO: Different modes (all network players, all network vehicles, network info, e.t.c.)
		DumpPlayer(g_pLocalPlayer);
	}
}
