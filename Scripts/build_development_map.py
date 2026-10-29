import unreal
from pathlib import Path
import json

level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
path = '/Game/Maps/L_Development'
if unreal.EditorAssetLibrary.does_asset_exist(path):
    raise RuntimeError('Development map already exists; refusing to overwrite authored work.')
level.new_level(path)
cube = unreal.load_asset('/Engine/BasicShapes/Cube.Cube')
material = unreal.load_asset('/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial')
def block(label, location, scale, physics=False):
    actor = actors.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(*location))
    actor.set_actor_label(label)
    component = actor.static_mesh_component
    if physics:
        component.set_mobility(unreal.ComponentMobility.MOVABLE)
    component.set_static_mesh(cube)
    component.set_material(0, material)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    component.set_collision_profile_name('PhysicsActor' if physics else 'BlockAll')
    if physics:
        component.set_simulate_physics(True)
        component.set_mass_override_in_kg('', 20 if 'Light' in label else 100, True)
    return actor
block('Floor', (0,0,-50), (32,24,1))
block('BackWall', (-1600,0,350), (1,24,8))
block('FrontWall', (1600,0,350), (1,24,8))
block('LeftWall', (0,-1200,350), (32,1,8))
block('RightWall', (0,1200,350), (32,1,8))
block('RaisedPlatform', (700,0,75), (5,6,1.5))
block('LowStep', (350,0,20), (2,6,0.4))
block('HighAnchorBeam', (0,0,650), (1,20,0.5))
block('SideAnchorBeam', (800,0,550), (0.5,18,0.5))
block('LightPhysicsCube_20kg', (0,-450,60), (1,1,1), True)
block('HeavyPhysicsCube_100kg', (250,-450,85), (1.5,1.5,1.5), True)
start = actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(-1100,0,100))
start.set_actor_label('PlayerStart')
sun = actors.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0,0,750), unreal.Rotator(-55,-30,0))
sun.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
sun.light_component.set_editor_property('intensity', 3.0)
sky = actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0,0,500))
sky.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
sky.light_component.set_editor_property('intensity', 1.0)
level.save_current_level()
unreal.EditorAssetLibrary.save_directory('/Game', only_if_is_dirty=True, recursive=True)
labels = [a.get_actor_label() for a in actors.get_all_level_actors()]
required = ['PlayerStart','Floor','HighAnchorBeam','LightPhysicsCube_20kg','HeavyPhysicsCube_100kg']
assert all(x in labels for x in required), labels
report = Path(unreal.Paths.project_saved_dir()) / 'Verification/map.json'
report.parent.mkdir(parents=True, exist_ok=True)
report.write_text(json.dumps({'map':path,'required_actors':required,'passed':True}, indent=2))
unreal.log('LINKAGE_MAP PASS')
