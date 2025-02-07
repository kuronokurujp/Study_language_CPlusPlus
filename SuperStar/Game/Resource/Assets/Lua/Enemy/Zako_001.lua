-- 呼び出せる関数一覧
-- EnemyEvent_Position(handle, x, y)
-- EnemyEvent_Die(handle)
-- EnemyEvent_Damage(handle)
--
-- 保存する変数一覧
local speed = 0.0

-- 初期化 --
function EnemyEvent_Init(in_uuHandle, in_fX, in_fY, in_fDirX, in_fDirY, in_fMoveSpeed)
    HE_LOG("初期化")
    speed = in_fMoveSpeed
end

-- 終了 --
function EnemyEvent_End(in_uuHandle)
    HE_LOG("終了")
end

-- 更新 --
function EnemyEvent_Update(in_uuHandle, in_fDt)
--    HE_LOG("更新")
    -- 浮動小数点ではなく整数として渡す
    local handleAsInteger = math.tointeger(in_uuHandle)
    EnemyEvent_Move(handleAsInteger, -(speed * 0.0), 0.0)
end

-- 画面外 --
function EnemyEvent_ScreenOut(in_uuHandle)
    HE_LOG("画面外")
end

-- ダメージ --
function EnemyEvent_Damage(in_uuHandle, in_sDamage)
    HE_LOG("ダメージ")
    -- ダメージ量に応じてエフェクトを出すとか
end
