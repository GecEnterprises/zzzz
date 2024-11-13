use std::sync::{Arc, RwLock};
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use crossbeam_channel::Receiver;
use std::f32::consts::PI;
use std::time::Duration;
use crate::shared::{AudioMessage, SharedState};

pub(crate) fn run_audio_thread(
    shared_state: Arc<RwLock<SharedState>>,
    audio_rx: Receiver<AudioMessage>,
) -> Result<(), Box<dyn std::error::Error>> {
    let host = cpal::default_host();
    let device = host.default_output_device()
        .ok_or("No output device available")?;

    let config = device.default_output_config()?;
    let sample_rate = config.sample_rate().0 as f32;
    let mut phase: f32 = 0.0;

    let state = shared_state.clone();
    let stream = device.build_output_stream(
        &config.into(),
        move |data: &mut [f32], _: &cpal::OutputCallbackInfo| {
            // Only read `shared_state` once per callback
            let state = state.read().unwrap();
            let (is_playing, volume, frequency) = {
                (state.is_playing, state.volume, state.frequency)
            };

            if is_playing {
                let phase_increment = 2.0 * PI * frequency / sample_rate;

                // Generate sine wave for each sample
                for sample in data.iter_mut() {
                    *sample = (phase.sin()) * volume;
                    phase = (phase + phase_increment) % (2.0 * PI);
                }
            } else {
                // Output silence when not playing
                for sample in data.iter_mut() {
                    *sample = 0.0;
                }
            }
        },
        move |err| eprintln!("Audio stream error: {}", err),
        Option::Some(Duration::from_secs(2)),
    )?;

    stream.play()?;

    // Handle messages from GUI thread
    while let Ok(message) = audio_rx.recv() {
        match message {
            AudioMessage::SetVolume(vol) => {
                shared_state.write().unwrap().volume = vol;
            }
            AudioMessage::SetFrequency(freq) => {
                shared_state.write().unwrap().frequency = freq;
            }
            AudioMessage::Play => {
                shared_state.write().unwrap().is_playing = true;
            }
            AudioMessage::Stop => {
                shared_state.write().unwrap().is_playing = false;
            }
        }
    }

    Ok(())
}
