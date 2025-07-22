import os

######################################################
def add_target(output_file, thickness_um, radius, material):
    content_target = f"""%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Target
    THICKNESS = {thickness_um} micrometer
    RADIUS    = {radius} mm
    MATERIAL  = {material}
    ANGLE     = 0 deg
    X         = 0 mm
    Y         = 0 mm
    Z         = 0 mm"""
    print(content_target, file=output_file)

######################################################
def add_x6(output_file, n_detectors, rho, z_position, flip):
    dphi = 360./n_detectors
    for iphi in range(0,n_detectors):
        phi = iphi*dphi
        content_x6 = f"""%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% X6 Forward E {iphi+1}/{n_detectors}
STARK
    Type = X6
    Rho  = {rho} mm
    Phi  = {phi} deg
    Z    = {z_position} mm
    Flip = {flip}"""
        print(content_x6, file=output_file)

######################################################
def add_qqq5(output_file, z_position, flip=1):
    for iphi in range(0,4):
        phi = 90*iphi
        content_qqq5 = f"""%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% QQQ5 Backward E {iphi}/4
STARK
    Type = QQQ5
    Pos  = 0 0 {z_position} mm
    Beta = {phi} deg
    Flip = {flip}"""
        print(content_qqq5, file=output_file)

######################################################
def make_p_elastic(particle_name, energy):
    reaction_conf_name = f"{particle_name}_elastic"
    reaction_file_name = f"inputs/{reaction_conf_name}.reac"
    reaction_file = open(reaction_file_name,'w')
    print(reaction_file_name)
    content = f"""%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%% Reaction file for {particle_name}+p elastic scattering %%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Beam
    Particle= {particle_name}
    Energy= 8 MeV
    SigmaEnergy= 0 MeV
    SigmaThetaX= 0 MeV
    SigmaPhiY= 0 MeV
    SigmaX= 0 MeV
    SigmaY= 0 MeV
    MeanThetaX= 0. deg
    MeanPhiY= 0 deg
    MeanX= 0 mm
    MeanY= 0 mm
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TwoBodyReaction
    Beam= {particle_name}
    Target= 1H
    Light= 1H
    Heavy= {particle_name}
    ExcitationEnergyLight= 0.0 MeV
    ExcitationEnergyHeavy= 0.0 MeV
    CrossSectionPath= flat.txt CS
    ShootLight= 1
    ShootHeavy= 1"""
    print(content, file=reaction_file)
    return reaction_file_name, reaction_conf_name

######################################################
def make_isotropic(particle_name, energy1, energy2, angle1, angle2):
    reaction_conf_name = f"{particle_name}_e{energy1}_{energy2}_a{angle1}_{angle2}"
    reaction_file_name = f"inputs/isotropic_{reaction_conf_name}.reac"
    reaction_file = open(reaction_file_name,'w')
    print(reaction_file_name)
    content = f"""%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%% Isotropic {particle_name} %%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Isotropic
    Particle                      = {particle_name}
    EnergyDistribution            = flat
    EnergyLow                     = {energy1} MeV
    EnergyHigh                    = {energy2} MeV
    HalfOpenAngleMin              = {angle1} deg
    HalfOpenAngleMax              = {angle2} deg
    x0                            = 0. mm
    y0                            = 0. mm
    z0                            = 0. mm
    ExcitationEnergy              = 0. MeV
    SigmaX                        = 0. mm
    SigmaY                        = 0. mm
    SigmaZ                        = 0. mm
    Multiplicity                  = 1
%    Direction                     =
%    EnergyDistributionHist        =
%    SourceProfile                 ="""
    print(content, file=reaction_file)
    return reaction_file_name, reaction_conf_name

######################################################
def make_detector_file(name):
    detector_file_name = f"inputs/stark_{name}.det"
    det_file = open(detector_file_name,'w')
    print()
    print(detector_file_name)
    return det_file

######################################################
def make_script(name):
    batch_file_name = f"batch_{name}.sh"
    batch_file = open(batch_file_name,'w')
    print(batch_file_name)
    viewer_file_name = f"viewer_{name}.sh"
    viewer_file = open(viewer_file_name,'w')
    print(viewer_file_name)
    os.chmod(batch_file_name, 0o755)
    os.chmod(viewer_file_name, 0o755)
    content = f"#!/bin/bash\n"
    print(content, file=batch_file)
    print(content, file=viewer_file)
    return batch_file, viewer_file

######################################################
def make_write_script(name, detector_file_name, reaction_file_name, reaction_conf_name, macro_for_sim="#", macro_for_ana="#"):
    batch_file, viewer_file = make_script(name)
    add_script(batch_file, viewer_file, name, detector_file_name, reaction_file_name, reaction_conf_name, macro_for_sim, macro_for_ana)

######################################################
def add_script(batch_file, viewer_file, name, detector_file_name, reaction_file_name, reaction_conf_name, macro_for_sim="#", macro_for_ana="#"):
    sim_file_name = f"stark_{name}.{reaction_conf_name}.sim.root"
    ana_file_name = f"stark_{name}.{reaction_conf_name}.ana.root"
    vwr_file_name = f"stark_{name}.{reaction_conf_name}.viewer.root"
    content_b = f"""tee > geant4_batch.mac <<EOF
/run/beamOn 100000
EOF\n
npsimulation --record-track -D {detector_file_name} -E {reaction_file_name} -O {sim_file_name} -B geant4_batch.mac
npanalysis -T data/{sim_file_name} SimulatedTree -O {ana_file_name}
{macro_for_sim} -- \\"data/{sim_file_name}\\"
{macro_for_ana} -- \\"data/{ana_file_name}\\"
"""
    content_v = f"""npsimulation -D {detector_file_name} -E {reaction_file_name} -O {vwr_file_name} -M geant4_vis.mac"""
    print(content_b, file=batch_file)
    print(content_v, file=viewer_file)
    #print(batch_file.name)
    #print(viewer_file.name)


if __name__ == "__main__":
    os.makedirs("inputs", exist_ok=True)
    os.makedirs("data", exist_ok=True)
    os.makedirs("figures", exist_ok=True)

    #################################################################################################
    name = "ko2421"
    det_file = make_detector_file(name)
    add_target(output_file=det_file, thickness_um=2.5, radius=10, material="CH2")
    add_x6    (output_file=det_file, n_detectors=12, rho= 95.0, z_position=101+0.5*75, flip=1)
    add_x6    (output_file=det_file, n_detectors=12, rho=102.0, z_position=101+0.5*75, flip=0)
    add_x6    (output_file=det_file, n_detectors=16, rho=126.5, z_position= 40+0.5*75, flip=1)
    reaction, conf = make_p_elastic(particle_name="40Ar", energy=8)
    make_write_script(name, det_file.name, reaction, conf, macro_for_ana="root draw_summary.C")

    #################################################################################################
    name = "ko2421_efficiency"
    det_file = make_detector_file(name)
    add_target(output_file=det_file, thickness_um=2.5, radius=10, material="CH2")
    add_x6    (output_file=det_file, n_detectors=12, rho= 95.0, z_position=101+0.5*75, flip=1)
    add_x6    (output_file=det_file, n_detectors=12, rho=102.0, z_position=101+0.5*75, flip=0)
    add_x6    (output_file=det_file, n_detectors=16, rho=126.5, z_position= 40+0.5*75, flip=1)
    reaction, conf = make_isotropic(particle_name="proton", energy1=10, energy2=10, angle1=0, angle2=90)
    make_write_script(name, det_file.name, reaction, conf, macro_for_ana="root draw_summary.C")

    #################################################################################################
    name = "ko2520"
    det_file = make_detector_file(name)
    add_target(output_file=det_file, thickness_um=2.5, radius=10, material="CH2")
    add_x6    (output_file=det_file, n_detectors=12, rho= 95.0, z_position=101+0.5*75, flip=1)
    add_x6    (output_file=det_file, n_detectors=12, rho=102.0, z_position=101+0.5*75, flip=0)
    add_x6    (output_file=det_file, n_detectors=16, rho=126.5, z_position= 40+0.5*75, flip=1)
    add_qqq5  (output_file=det_file, z_position=160)
    reaction, conf = make_p_elastic(particle_name="21Na", energy=8)
    reaction, conf = make_p_elastic(particle_name="25Na", energy=8)
    make_write_script(name, det_file.name, reaction, conf)

    #################################################################################################
    name = "ko2520"
    det_file = make_detector_file(name)
    add_target(output_file=det_file, thickness_um=2.5, radius=10, material="CH2")
    add_x6    (output_file=det_file, n_detectors=12, rho= 95.0, z_position=101+0.5*75, flip=1)
    add_x6    (output_file=det_file, n_detectors=12, rho=102.0, z_position=101+0.5*75, flip=0)
    add_x6    (output_file=det_file, n_detectors=16, rho=126.5, z_position= 40+0.5*75, flip=1)
    add_qqq5  (output_file=det_file, z_position=160)
    reaction, conf = make_isotropic(particle_name="proton", energy1=10, energy2=10, angle1=0, angle2=90)
    make_write_script(name, det_file.name, reaction, conf, macro_for_ana="root draw_summary.C")

    #################################################################################################
    name = "qqq5"
    det_file = make_detector_file(name)
    add_target(output_file=det_file, thickness_um=2.5, radius=10, material="CH2")
    add_qqq5  (output_file=det_file, z_position=160)
    add_qqq5  (output_file=det_file, z_position=180)
    batch_file, viewer_file = make_script(name)
    reaction, conf = make_isotropic(particle_name="proton", energy1=5, energy2=30, angle1=5, angle2=30)
    add_script(batch_file, viewer_file, name, det_file.name, reaction, conf, macro_for_sim="root -q create_tree_for_qqq5.C")
    reaction, conf = make_isotropic(particle_name="2H",     energy1=5, energy2=30, angle1=5, angle2=30)
    add_script(batch_file, viewer_file, name, det_file.name, reaction, conf, macro_for_sim="root -q create_tree_for_qqq5.C")
