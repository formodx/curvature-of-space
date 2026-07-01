#include <math.h>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


float planet_mass = 0.0f;
const int grid_size = 2, grid_step = 20;
const float planet_radius = 4.0f;


Vector3 get_distorted_position(Vector3 point){
    float distance = Vector3Length(point);
    if(distance < planet_radius) return point;

    float distortion_strength = planet_mass / (distance*distance+planet_mass);
    float new_distance = distance * (1.0f-distortion_strength);
    new_distance = fmaxf(new_distance, planet_radius);

    return Vector3Scale(Vector3Normalize(point), new_distance);
}


Color color_lerp(Color color1, Color color2, float amount){
    amount = Clamp(amount, 0.0f, 1.0f);

    return (Color) {
        (unsigned char) (color1.r+(color2.r-color1.r)*amount),
        (unsigned char) (color1.g+(color2.g-color1.g)*amount),
        (unsigned char) (color1.b+(color2.b-color1.b)*amount),
        (unsigned char) (color1.a+(color2.a-color1.a)*amount)
    };
}


Color get_distortion_color(Vector3 point){
    float distance = Vector3Length(point);
    float intensity = planet_mass / (distance*distance+planet_mass);

    Color far_color = (Color) {55, 190, 220, 255};
    Color near_color = (Color) {255, 135, 80, 255};
    return color_lerp(far_color, near_color, powf(intensity, 0.4f));
}


void draw_line(Vector3 point1, Vector3 point2){
    const int line_segments = 32;
    Vector3 previous_original = point1;
    Vector3 previous = get_distorted_position(point1);
    for(float i=1.0f; i<=line_segments; ++i){
        Vector3 current_original = Vector3Lerp(point1, point2, i / line_segments);
        Vector3 current = get_distorted_position(current_original);

        float distance1 = Vector3Length(previous);
        float distance2 = Vector3Length(current);
        float cutoff = planet_radius + 0.01f;
        if(distance1>cutoff && distance2>cutoff){
            Vector3 middle = Vector3Lerp(previous_original, current_original, 0.5f);
            Color color = get_distortion_color(middle);

            DrawLine3D(previous, current, color);
        }

        previous_original = current_original;
        previous = current;
    }
}


void draw_x(){
    float limit = grid_size * grid_step;
    for(float y=-limit; y<=limit; y+=grid_step){
        for(float z=-limit; z<=limit; z+=grid_step){
            for(float x=-limit; x<limit; x+=grid_step){
                draw_line((Vector3) {x, y, z}, (Vector3) {x + grid_step, y, z});
            }
        }
    }
}


void draw_y(){
    float limit = grid_size * grid_step;
    for(float x=-limit; x<=limit; x+=grid_step){
        for(float z=-limit; z<=limit; z+=grid_step){
            for(float y=-limit; y<limit; y+=grid_step){
                draw_line((Vector3) {x, y, z}, (Vector3) {x, y + grid_step, z});
            }
        }
    }
}


void draw_z(){
    float limit = grid_size * grid_step;
    for(float x=-limit; x<=limit; x+=grid_step){
        for(float y=-limit; y<=limit; y+=grid_step){
            for(float z=-limit; z<limit; z+=grid_step){
                draw_line((Vector3) {x, y, z}, (Vector3) {x, y, z + grid_step});
            }
        }
    }
}


int main(){
    const int screen_width = 1280;
    const int screen_height = 720;

    InitWindow(screen_width, screen_height, "window");
    SetTargetFPS(60);

    Camera3D camera;
    camera.position = (Vector3) {80.0f, 50.0f, 80.0f};
    camera.target = (Vector3) {0.0f, 0.0f, 0.0f};
    camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
    camera.fovy = 50.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Rectangle bounds;
    bounds.width = 600.0f;
    bounds.height = 40.0f;
    bounds.x = (screen_width-bounds.width) / 2.0f;
    bounds.y = screen_height - bounds.height*2.0f;

    while(!WindowShouldClose()){
        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
        ClearBackground((Color) {5, 8, 18, 255});
        BeginMode3D(camera);

        draw_x();
        draw_y();
        draw_z();
        DrawSphere(
            (Vector3) {0.0f, 0.0f, 0.0f},
            planet_radius,
            (Color) {255, 220, 90, 255}
        );

        EndMode3D();

        GuiSliderBar(
            bounds,
            "mass",
            TextFormat("%.1f", planet_mass),
            &planet_mass,
            0.0f,
            1000.0f
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}