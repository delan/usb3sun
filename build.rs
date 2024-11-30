use std::error::Error;

fn main() -> Result<(), Box<dyn Error>> {
    let scons_variables = embuild::pio::project::SconsVariables::from_piofirst()
        .expect("Failed to get scons variables");
    let factory = embuild::bindgen::Factory::from_scons_vars(&scons_variables)?;

    // thread 'main' panicked at [...]/.cargo/registry/src/index.crates.io-6f17d22bba15001f/bindgen-0.69.5/ir/context.rs:556:15:
    // libclang error; [...]
    // embuild::bindgen::run(factory.builder()?)?;
    drop(factory);

    Ok(())
}
