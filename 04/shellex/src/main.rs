use std::process::{Command, Stdio, Child};
use std::io::{stdin, stdout, Write};
use std::env;

fn main() {
    loop {
        print!("> ");
        stdout().flush().unwrap();

        let mut input = String::new();
        stdin().read_line(&mut input).unwrap();
        let input = input.trim();

        if input == "quit" {
            break;
        }

        let (command, args) = parse_line(&input);
        if command.is_empty() {
            continue;
        }

        let background = args.contains(&"&");
        execute_command(command, args, background);
    }
}

fn parse_line(input: &str) -> (&str, Vec<&str>) {
    let mut parts = input.split_whitespace();
    let command = parts.next().unwrap_or_default();
    let args = parts.collect::<Vec<&str>>();
    (command, args)
}

fn execute_command(command: &str, args: Vec<&str>, background: bool) {
    let mut child = Command::new(command)
        .args(&args)
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .spawn()
        .expect("failed to execute command");

    if !background {
        child.wait().expect("failed to wait on child");
    } else {
        println!("Started {} in background", child.id());
    }
}

