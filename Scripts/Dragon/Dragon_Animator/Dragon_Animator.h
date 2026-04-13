#ifndef DRAGON_ANIMATOR_H
#define DRAGON_ANIMATOR_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <map>

namespace godot 
{
    enum DragonSpecies
    {
        SPECIES_TOOTHLESS,
        SPECIES_GRONCKLE
    };

    class Dragon_Animator : public Node // extends the Node class
    {
        GDCLASS(Dragon_Animator, Node);

        public:
            Dragon_Animator();  // constructor
            ~Dragon_Animator(); // destructor
            void _ready() override;
            void _physics_process(float delta);
            void RefreshBindings();
            void SetAnimation(const String &layer, const String &animation, bool freeze = false);
            void Unfreeze();
            void SetAnimation_Mouth(int step, float thresh);
            void SetAnimation_Weight(const String &layer, float weight);
            
        protected:
            static void _bind_methods();
        
        private:
            AnimationTree* anim_tree = nullptr;
            std::map<String, AnimationNodeStateMachinePlayback*> layer_map;
            AnimationPlayer* anim_player = nullptr;
            std::map<String, double> original_anim_length; // a map to store the original length of animations
            String last_frozen_layer; // the last layer that was frozen
            String last_frozen_animation; // the last animation that was frozen
            int dragon_species = SPECIES_TOOTHLESS; // default to Toothless
            int animation_mouth_step = 0; 
            float animation_mouth_thresh = 0.0f;
            float animation_mouth_weight = 1.0f;
    };
}

VARIANT_ENUM_CAST(godot::DragonSpecies); // use godot macro (VARIANT_ENUM_CAST) to register the enum values in the engine

# endif // DRAGON_ANIMATOR_H