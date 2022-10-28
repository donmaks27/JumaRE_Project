var path = require('path');
var fs = require('fs');
var child_process = require('child_process');

var current_dir = __dirname;

var compile_bin_path_file = fs.readFileSync(current_dir + "\\compile_bin_path.json", { encoding: "utf-8" });
/** @type {{ GLSL_to_SPIRV: string, SPIRV_to_HLSL: string, HLSL: string }} */
var compile_bin_path_json = JSON.parse(compile_bin_path_file);
if (!compile_bin_path_json.GLSL_to_SPIRV || !compile_bin_path_json.SPIRV_to_HLSL || !compile_bin_path_json.HLSL) {
    console.log("Empty compile_bin_path.json file");
    return;
}

var directories_to_check = [ current_dir ];
while (true) {
    var dir_name = directories_to_check.shift();
    if (!dir_name) {
        break;
    }

    var files = fs.readdirSync(dir_name, { withFileTypes: true });
    for (var index = 0; index < files.length; index++) {
        var file = files[index];
        var file_name = dir_name + '/' + file.name;

        if (file.isDirectory()) {
            directories_to_check.push(file_name);
            continue;
        }

        if (file_name.endsWith('.vert')) {
            var compile_SPIRV_command = compile_bin_path_json.GLSL_to_SPIRV + " " + file_name + " -o " + file_name + ".spv";
            var compile_HLSL_source_command = compile_bin_path_json.SPIRV_to_HLSL + " --hlsl --entry main --stage vert --shader-model 50 " + file_name + ".spv --output " + file_name + ".hlsl";
            var compile_HLSL_command = compile_bin_path_json.HLSL + " /T vs_5_0 /E main /Fo " + file_name + ".hlsl.obj /nologo " + file_name + ".hlsl";
            child_process.execSync(compile_SPIRV_command);
            child_process.execSync(compile_HLSL_source_command);
            child_process.execSync(compile_HLSL_command);
        }
        else if (file_name.endsWith('.frag')) {
            var compile_SPIRV_command = compile_bin_path_json.GLSL_to_SPIRV + " " + file_name + " -o " + file_name + ".spv";
            var compile_HLSL_source_command = compile_bin_path_json.SPIRV_to_HLSL + " --hlsl --entry main --stage frag --shader-model 50 " + file_name + ".spv --output " + file_name + ".hlsl";
            var compile_HLSL_command = compile_bin_path_json.HLSL + " /T ps_5_0 /E main /Fo " + file_name + ".hlsl.obj /nologo " + file_name + ".hlsl";
            child_process.execSync(compile_SPIRV_command);
            child_process.execSync(compile_HLSL_source_command);
            child_process.execSync(compile_HLSL_command);
        }
    }
}