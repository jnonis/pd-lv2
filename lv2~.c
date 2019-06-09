//
//  lv2~.c
//  pd-lv2~
//
//  Created by javiernonis on 5/9/19.
//  Copyright © 2019 jnonis. All rights reserved.
//

#include "m_pd.h"
#include <stdlib.h>
#include <string.h>

static t_class *lv2_tilde_class;

typedef struct _lv2_tilde {
    t_object x_obj;
    t_sample f_dummy;
    
    char* plugin_uri;
    
    t_int* dsp_vec;
    unsigned dsp_vec_length;
    
    t_outlet *x_out;
    
    t_int num_audio_inputs;
    t_int num_audio_outputs;
    t_inlet** audio_inlets;
    t_outlet** audio_outlets;
} t_lv2_tilde;

t_int *lv2_tilde_perform(t_int *w) {
    t_lv2_tilde *x = (t_lv2_tilde *) (w[1]);
    t_sample *in_left = (t_sample *) (w[2]);
    t_sample *in_right = (t_sample *) (w[3]);
    t_sample *out_left = (t_sample *) (w[4]);
    t_sample *out_right = (t_sample *) (w[5]);
    int n = (int) (w[6]);
    
    
    return (w + 7);
}

void lv2_tilde_dsp(t_lv2_tilde *x, t_signal **sp) {
    dsp_add(lv2_tilde_perform,
            6,
            x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[2]->s_vec,
            sp[3]->s_vec,
            sp[0]->s_n);
}

void lv2_tilde_free(t_lv2_tilde *x) {
    if (x->audio_inlets != NULL) {
        for (int i = 0; i < x->num_audio_inputs; i++) {
            inlet_free(x->audio_inlets[i]);
        }
        free(x->audio_inlets);
        x->audio_inlets = NULL;
    }
    
    if (x->audio_outlets != NULL) {
        for (int i = 0; i < x->num_audio_outputs; i++) {
            outlet_free(x->audio_outlets[i]);
        }
        free (x->audio_outlets);
        x->audio_outlets = NULL;
    }
    
    outlet_free(x->x_out);
}

void *lv2_tilde_new(t_symbol* s_name) {
    t_lv2_tilde *x = (t_lv2_tilde *)pd_new(lv2_tilde_class);

    if (s_name->s_name != NULL) {
        x->plugin_uri = s_name->s_name;
        post(x->plugin_uri);
        
        // TODO: load plugin
        
        x->num_audio_inputs = 2;
        x->num_audio_outputs = 2;
    } else {
        pd_error(x, "lv2~: lv2 plugin plugin uri is missing");
        
        x->num_audio_inputs = 2;
        x->num_audio_outputs = 2;
    }
    
    // Control outlet
    x->x_out = outlet_new(&x->x_obj, &s_symbol);

    // Allocate memory for inlet and outlet pointers
    x->audio_inlets = (t_inlet**) calloc(x->num_audio_inputs, sizeof(t_inlet*));
    x->audio_outlets = (t_outlet**) calloc(x->num_audio_outputs, sizeof(t_outlet*));
    
    for (int i = 0; i < x->num_audio_inputs; i++) {
        x->audio_inlets[i] = inlet_new (&x->x_obj,
                                        &x->x_obj.ob_pd,
                                        gensym ("signal"),
                                        gensym ("signal"));
    }
    for (int i = 0; i < x->num_audio_outputs; i++) {
        x->audio_outlets[i] = outlet_new (&x->x_obj, gensym ("signal"));
    }

    return (void *)x;
}

void lv2_tilde_control(t_lv2_tilde *x, t_symbol* name, t_float value) {
    if (name->s_name == NULL || strlen (name->s_name) == 0) {
        pd_error(x, "lv2~: control messages must have a name and a value");
        return;
    }
    
    post("Control %s: %f", name->s_name, value);
    outlet_symbol(x->x_out, name);
}

void lv2_tilde_info(t_lv2_tilde *x) {
    // TODO: Get plugin info
    post("Info");
    outlet_symbol(x->x_out, gensym("Info"));
}

void lv2_tilde_setup(void) {
    lv2_tilde_class = class_new(gensym("lv2~"),
                                 (t_newmethod)lv2_tilde_new,
                                 (t_method)lv2_tilde_free,
                                 sizeof(t_lv2_tilde),
                                 CLASS_DEFAULT,
                                 A_DEFSYM,
                                 0);
    
    class_addmethod(lv2_tilde_class,
                    (t_method)lv2_tilde_dsp,
                    gensym("dsp"),
                    A_NULL);
    CLASS_MAINSIGNALIN(lv2_tilde_class, t_lv2_tilde, f_dummy);
    
    class_addmethod (lv2_tilde_class,
                     (t_method)lv2_tilde_control,
                     gensym ("control"),
                     A_SYMBOL,
                     A_FLOAT,
                     0);
    
    class_addmethod (lv2_tilde_class,
                     (t_method)lv2_tilde_info,
                     gensym ("info"),
                     0);
}
