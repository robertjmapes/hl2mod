//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
    #include "c_hl2mp_player.h"
#else
    #include "grenade_ar2.h"
    #include "hl2mp_player.h"
    #include "basegrenade_shared.h"
#endif

#include "weapon_hl2mpbase.h"
#include "weapon_hl2mpbase_machinegun.h"

#ifdef CLIENT_DLL
#define CWeaponAK47 C_WeaponAK47
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


class CWeaponAK47 : public CHL2MPMachineGun
{
public:
    DECLARE_CLASS( CWeaponAK47, CHL2MPMachineGun );

    CWeaponAK47();

    DECLARE_NETWORKCLASS(); 
    DECLARE_PREDICTABLE();

    
    void    Precache( void );
    void    AddViewKick( void );

    int        GetMinBurst() { return 2; }
    int        GetMaxBurst() { return 5; }

    virtual void Equip( CBaseCombatCharacter *pOwner );
    bool    Reload( void );

    float    GetFireRate( void ) { return 0.075f; }    // 13.3hz
    Activity    GetPrimaryAttackActivity( void );

    virtual const Vector& GetBulletSpread( void )
    {
        static const Vector cone = VECTOR_CONE_5DEGREES;
        return cone;
    }

    const WeaponProficiencyInfo_t *GetProficiencyValues();

#ifndef CLIENT_DLL
    DECLARE_ACTTABLE();
#endif
    
private:
    CWeaponAK47( const CWeaponAK47 & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponAK47, DT_WeaponAK47 )

BEGIN_NETWORK_TABLE( CWeaponAK47, DT_WeaponAK47 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponAK47 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_ak47, CWeaponAK47 );
PRECACHE_WEAPON_REGISTER(weapon_ak47);

#ifndef CLIENT_DLL
acttable_t    CWeaponAK47::m_acttable[] = 
{
    { ACT_HL2MP_IDLE,                    ACT_HL2MP_IDLE_SMG1,                    false },
    { ACT_HL2MP_RUN,                    ACT_HL2MP_RUN_SMG1,                        false },
    { ACT_HL2MP_IDLE_CROUCH,            ACT_HL2MP_IDLE_CROUCH_SMG1,                false },
    { ACT_HL2MP_WALK_CROUCH,            ACT_HL2MP_WALK_CROUCH_SMG1,                false },
    { ACT_HL2MP_GESTURE_RANGE_ATTACK,    ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,    false },
    { ACT_HL2MP_GESTURE_RELOAD,            ACT_HL2MP_GESTURE_RELOAD_SMG1,            false },
    { ACT_HL2MP_JUMP,                    ACT_HL2MP_JUMP_SMG1,                    false },
    { ACT_RANGE_ATTACK1,                ACT_RANGE_ATTACK_SMG1,                    false },
};

IMPLEMENT_ACTTABLE(CWeaponAK47);
#endif

//=========================================================
CWeaponAK47::CWeaponAK47( )
{
    m_fMinRange1        = 0;// No minimum range. 
    m_fMaxRange1        = 1400;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponAK47::Precache( void )
{
#ifndef CLIENT_DLL
    UTIL_PrecacheOther("grenade_ar2");
#endif

    BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Give this weapon longer range when wielded by an ally NPC.
//-----------------------------------------------------------------------------
void CWeaponAK47::Equip( CBaseCombatCharacter *pOwner )
{
    m_fMaxRange1 = 1400;

    BaseClass::Equip( pOwner );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponAK47::GetPrimaryAttackActivity( void )
{
    if ( m_nShotsFired < 2 )
        return ACT_VM_PRIMARYATTACK;

    if ( m_nShotsFired < 3 )
        return ACT_VM_RECOIL1;
    
    if ( m_nShotsFired < 4 )
        return ACT_VM_RECOIL2;

    return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponAK47::Reload( void )
{
    bool fRet;
    float fCacheTime = m_flNextSecondaryAttack;

    fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
    if ( fRet )
    {
        // Undo whatever the reload process has done to our secondary
        // attack timer. We allow you to interrupt reloading to fire
        // a grenade.
        m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;

        WeaponSound( RELOAD );
    }

    return fRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponAK47::AddViewKick( void )
{
    #define    EASY_DAMPEN            0.5f
    #define    MAX_VERTICAL_KICK    1.0f    //Degrees
    #define    SLIDE_LIMIT            2.0f    //Seconds
    
    //Get the view kick
    CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

    if ( pPlayer == NULL )
        return;

    DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}


//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CWeaponAK47::GetProficiencyValues()
{
    static WeaponProficiencyInfo_t proficiencyTable[] =
    {
        { 7.0,        0.75    },
        { 5.00,        0.75    },
        { 10.0/3.0, 0.75    },
        { 5.0/3.0,    0.75    },
        { 1.00,        1.0        },
    };

    COMPILE_TIME_ASSERT( ARRAYSIZE(proficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

    return proficiencyTable;
}
