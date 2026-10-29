"""Restore attack wiring after mesh/montage replacement. Use -VerifyEnemyAttacks to check only."""
import json
import shutil
from pathlib import Path
import unreal

ROOT = '/Game/Assets/NewEnemy/'
MELEE = '/Game/Enemy/Animation/ANS_EnemyNotify_Melee.ANS_EnemyNotify_Melee_C'
ELITE = '/Game/Enemy/Animation/ANS_EnemyNotify_Elite.ANS_EnemyNotify_Elite_C'
RANGED = '/Game/Enemy/Animation/ANS_EnemyNotify_Range.ANS_EnemyNotify_Range_C'
VERIFY = '-VerifyEnemyAttacks' in unreal.SystemLibrary.get_command_line()
BACKUP = Path(unreal.Paths.project_saved_dir()) / 'AnimationBackups' / 'EnemyAttackRepair'
CHANGED = []

def load(p, kind=None):
    obj = unreal.load_asset(p)
    assert obj and (kind is None or isinstance(obj, kind)), p
    return obj

def backup(obj):
    if VERIFY:
        return
    relative = obj.get_outermost().get_name().removeprefix('/Game/') + '.uasset'
    source = Path(unreal.Paths.project_content_dir()) / relative
    target = BACKUP / relative
    target.parent.mkdir(parents=True, exist_ok=True)
    if source.exists() and not target.exists():
        shutil.copy2(source, target)

def save(obj):
    if VERIFY:
        return
    assert unreal.EditorAssetLibrary.save_loaded_asset(obj, only_if_is_dirty=False), obj.get_path_name()
    CHANGED.append(obj.get_path_name())

def attack(path, notify_class, start, duration):
    montage = load(ROOT + path, unreal.AnimMontage)
    cls = unreal.load_class(None, notify_class)
    assert cls
    events = unreal.AnimationLibrary.get_animation_notify_events(montage)
    matching = [e for e in events if e.get_editor_property('notify_state_class') and
                e.get_editor_property('notify_state_class').get_class() == cls]
    if not VERIFY:
        backup(montage)
        assert unreal.NewEnemyAnimationLibrary.set_montage_slot(montage, 'DefaultSlot')
        if not matching:
            tracks = unreal.AnimationLibrary.get_animation_notify_track_names(montage)
            if 'Combat' not in [str(t) for t in tracks]:
                unreal.AnimationLibrary.add_animation_notify_track(montage, 'Combat')
            unreal.AnimationLibrary.add_animation_notify_state_event(montage, 'Combat', start, duration, cls)
        save(montage)
    else:
        assert len(matching) == 1, montage.get_path_name() + ': missing/duplicate attack notify'
    return montage

shadow = attack('ShadowKight/Animations/AM_shadowknightAttack', MELEE, 0.30, 0.40)
golem = attack('StoneGolem/Animations/AM_GolemAttack', ELITE, 0.50, 0.65)
golem_special = attack('StoneGolem/Animations/AM_GolemAttackspecial', ELITE, 0.80, 0.85)
tree = attack('PackOfTreeEnts/TreeEntAsh/Animations/UE5/AM_TreeAttack', MELEE, 0.60, 0.65)
kraken = attack('Kraken/Animations/AM_KrakenAttack', MELEE, 0.70, 0.65)
kraken_special = load(ROOT + 'Kraken/Animations/AM_KrakenAttack2', unreal.AnimMontage)
tree_special = load(ROOT + 'PackOfTreeEnts/TreeEntAsh/Animations/UE5/AM_TreeSpecial', unreal.AnimMontage)

sevarog_path = ROOT + 'Sevarog/Animations/AM_SevarogRangedAttack'
if not VERIFY and not unreal.EditorAssetLibrary.does_asset_exist(sevarog_path):
    assert unreal.EditorAssetLibrary.duplicate_asset(ROOT + 'Sevarog/Animations/Swing1_Medium_Montage', sevarog_path)
sevarog = attack('Sevarog/Animations/AM_SevarogRangedAttack', RANGED, 0.35, 0.08)

# The tree Blueprint starts its projectile pattern from PlayMontage's OnNotifyBegin.
for montage in [tree_special, kraken_special]:
    if not VERIFY:
        backup(montage)
        assert unreal.NewEnemyAnimationLibrary.set_montage_slot(montage, 'DefaultSlot')
    events = unreal.AnimationLibrary.get_animation_notify_events(montage)
    cls = unreal.AnimNotify_PlayMontageNotifyWindow
    matching = [e for e in events if isinstance(e.get_editor_property('notify_state_class'), cls)]
    if not VERIFY:
        if not matching:
            if 'Combat' not in [str(t) for t in unreal.AnimationLibrary.get_animation_notify_track_names(montage)]:
                unreal.AnimationLibrary.add_animation_notify_track(montage, 'Combat')
            notify = unreal.AnimationLibrary.add_animation_notify_state_event(montage, 'Combat', 0.30, 0.10, cls)
            notify.set_editor_property('notify_name', 'FirePattern')
        save(montage)
    else:
        assert len(matching) == 1, montage.get_path_name() + ': missing projectile pattern notify'

for name, primary, special, socket in [
    ('BP_Assassin', shadow, None, None),
    ('BP_EliteEnemy', golem, golem_special, None),
    ('BP_TreeEliteEnemy', tree, tree_special, 'hand_r'),
    ('BP_TreeEliteEnemy1', kraken, kraken_special, 'KRAKEN_FOREARM_R'),
    ('BP_Archer', sevarog, None, 'hand_r'),
]:
    bp = load('/Game/Enemy/Blueprint/' + name, unreal.Blueprint)
    if not VERIFY:
        backup(bp)
        if special:
            old = load('/Game/Enemy/Animation/Tree/AM_TreeSkill')
            count = unreal.NewEnemyAnimationLibrary.replace_graph_object_reference(bp, old, special)
            unreal.log('ENEMY_REPAIR_GRAPH ' + name + ': ' + str(count))
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    cls = unreal.BlueprintEditorLibrary.generated_class(bp)
    cdo = unreal.get_default_object(cls)
    if not VERIFY:
        cdo.set_editor_property('attack_montage', primary)
        if special:
            cdo.set_editor_property('special_attack_montage', special)
        if socket:
            cdo.set_editor_property('weapon_socket_name', socket)
        save(bp)
    mesh = cdo.get_component_by_class(unreal.SkeletalMeshComponent)
    skeleton = mesh.get_editor_property('skeletal_mesh_asset').get_editor_property('skeleton')
    assert cdo.get_editor_property('attack_montage') == primary
    assert primary.get_editor_property('skeleton') == skeleton, name + ': incompatible attack'
    assert mesh.does_socket_exist(str(cdo.get_editor_property('weapon_socket_name'))), name + ': invalid socket'
    if special:
        assert cdo.get_editor_property('special_attack_montage') == special
        assert special.get_editor_property('skeleton') == skeleton
    unreal.log('ENEMY_REPAIR_OK ' + name)

unreal.log('ENEMY_REPAIR_COMPLETE ' + json.dumps(dict(verify=VERIFY, saved=CHANGED)))
